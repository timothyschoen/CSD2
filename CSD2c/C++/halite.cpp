#pragma once
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include "AudioFile.h"
// temporary for calculating which functions are slow
#include <chrono>


namespace
{

// gMin for diodes etc..
constexpr double gMin = 1e-12;

// voltage tolerance
constexpr double vTolerance = 5e-5;

// thermal voltage for diodes/transistors
constexpr double vThermal = 0.026;

constexpr unsigned maxIter = 200;

constexpr bool VERBOSE_LU = false;

constexpr int unitValueOffset = 4;
constexpr int unitValueMax = 8;
constexpr const char* unitValueSuffixes[] = {
    "p", "n", "u", "m", "", "k", "M", "G"
};


}

//
// General overview
// ----------------
//
// Circuits are built from nodes and Components, where nodes are
// simply positive integers (with 0 designating ground).
//
// Every Component has one or more pins connecting to the circuit
// nodes as well as zero or more internal nets.
//
// While we map pins directly to nets here, the separation would
// be useful if the solver implemented stuff like net-reordering.
//
// MNACell represents a single entry in the solution matrix,
// where we store constants and time-step dependent constants
// separately, plus collect pointers to dynamic variables.
//
// We track enough information here that we only need to stamp once.
//
struct MNACell
{
    double g;       // simple values (eg. resistor conductance)
    double gtimed;  // time-scaled values (eg. capacitor conductance)

    // pointers to dynamic variables, added in once per solve
    std::vector<double*>    gdyn;
    std::vector<double*>    gdyntimed;

    double  lu, prelu;  // lu-solver values and matrix pre-LU cache

    void clear()
    {
        g = 0;
        gtimed = 0;
    }

    void initLU(double stepScale)
    {
        prelu = g + gtimed * stepScale;


    }

    // restore matrix state and update dynamic values
    void updatePre(double stepScale)
    {
        lu = prelu;
        for(int i = 0; i < gdyn.size(); ++i)
        {
            lu += *(gdyn[i]);
        }
        for(int i = 0; i < gdyntimed.size(); ++i)
        {
            lu += *(gdyntimed[i]) * stepScale;
        }
    }
};

// this is for keeping track of node information
// for the purposes of more intelligent plotting
struct MNANodeInfo
{
    enum Type
    {
        tVoltage,
        tCurrent,

        tCount
    };

    Type    type;   // one auto-range per unit-type
    double  scale;  // scale factor (eg. charge to voltage)

    std::string name;   // node name for display
};

// Stores A and b for A*x - b = 0, where x is the solution.            !!!!!!!!!!!!!!!!!!!

// A en B zijn dus matrix A en Z?? Want X is normaal de solution
// Wat krijg je dan eigenlijk met a.lu en b.lu? (bonusvraag waarom is A.lu altijd 1, 0 of -1 wait dat is dan de Z matrix??? maar a.g dan???/)
// A is stored as a vector of rows, for easy in-place pivots
//
struct MNASystem
{
    typedef std::vector<MNACell>    MNAVector;
    typedef std::vector<MNAVector>  MNAMatrix;

    // node names - for output
    std::vector<MNANodeInfo>    nodes;

    MNAMatrix   A; // Standard A matrix
    MNAVector   b; // This is likely the Z matrix since that contains our known values
    //  Wikipedia: "When solving systems of equations, b is usually treated as a vector with a length equal to the height of matrix A"

    double      time;
    long      ticks;

    std::vector<double> digiValues;

    void setSize(int n, int d)
    {
        A.resize(n);
        b.resize(n);
        digiValues.resize(d);

        nodes.resize(n);

        for(unsigned i = 0; i < n; ++i)
        {
            b[i].clear(); // keeping it 2d
            A[i].resize(n); // making it 3d

            nodes[i].type = MNANodeInfo::tVoltage;
            nodes[i].scale = 1;

            for(unsigned j = 0; j < n; ++j)
            {
                A[i][j].clear();
            }
        }

        time = 0;
        ticks = 0;
    }

    void stampTimed(double g, int r, int c)
    {
        A[r][c].gtimed += g;
    }


    void stampStatic(double g, int r, int c)
    {
        A[r][c].g += g;
    }

    // this doesn't work!
    void stampConductor(double g, int r, int c)
    {
        A[r][r].g += g;
        A[r][c].g -= g;
        A[c][r].g -= g;
        A[c][c].g += g;
    }

    void setDigital(std::vector<int> outputs, double value)
    {
        for (size_t i = 0; i < outputs.size(); i++) {
            digiValues[outputs[i]] = value;
        }

    }

    double getDigital(std::vector<int> inputs, double fallback = 0)
    {
        double accum = 0;
        if (inputs.size() > 0) {
            for (size_t i = 0; i < inputs.size(); i++) {
                accum += digiValues[inputs[i]];
            }
        }
        else {
            accum = fallback;
        }
        return accum;
    }


};

struct IComponent
{
    virtual ~IComponent() {}

    // return the number of pins for this component
    virtual int pinCount() = 0;

    // return a pointer to array of pin locations
    // NOTE: these will eventually be GUI locations to be unified
    virtual const int* getPinLocs() const = 0;
    virtual const std::vector<std::string> getDigiLocs() const = 0;

    // setup pins and calculate the size of the full netlist
    // the Component<> will handle this automatically
    //
    //  - netSize is the current size of the netlist
    //  - pins is an array of circuits nodes
    //
    virtual void setupNets(int & netSize, int & states, const int* pins, const std::vector<std::string> digiPins) = 0;

    virtual void digitalReset(const int* digiPins) = 0;


    // stamp constants into the matrix
    virtual void stamp(MNASystem & m) = 0;

    // this is for allocating state variables
    virtual void setupStates(int & states) {}

    // update state variables, only tagged nodes
    // this is intended for fixed-time compatible
    // testing to make sure we can code-gen stuff
    virtual void update(MNASystem & m) {}
    virtual void updateInput(MNASystem & m) {}

    // return true if we're done - will keep iterating
    // until all the components are happy
    virtual bool newton(MNASystem & m) {
        return true;
    }

    // NEW: output function for probe object;
    virtual double getAudioOutput(MNASystem & m, int c) {
        return 0;
    }

    // realtime input function;
    virtual void setAudioInput(MNASystem & m, double input) {}

    virtual void setMidiInput(MNASystem & m, std::vector<unsigned char> message) {}

    // time-step change, for caps to fix their state-variables
    virtual void scaleTime(double told_per_new) {}
};

template <int nPins = 0, int nInternalNets = 0, int nDigipins = 0>
struct Component : IComponent
{
    static const int nNets = nPins + nInternalNets;

    int pinLoc[nPins];
    int nets[nNets];
    std::vector<std::vector<int>> digiNets = std::vector<std::vector<int>>(nDigipins);
    std::vector<std::string> digiPins = std::vector<std::string>(nDigipins);

    //std::string digiNets[nDigipins];

    int pinCount() final {
        return nPins;
    }

    const int* getPinLocs() const final {
        return pinLoc;    // This is used for running setupnets!!!! (for linking pinloc to nets)
    }
    const std::vector<std::string> getDigiLocs() const final {
        return digiPins;
    }


    void setupNets(int & netSize, int & states, const int* pins, const std::vector<std::string> digipins) final
    {
        for(int i = 0; i < nPins; ++i)
        {
            nets[i] = pins[i];
        }

        for(int i = 0; i < nDigipins; ++i)
        {
            std::string connections = digipins[i];
            connections.erase(0, 1);
            connections.erase(connections.size()-1, 1);
            std::stringstream ss(connections);
            std::string obj;
            std::vector<int> inputs;
            if(connections.size() > 2) {

                while(std::getline(ss,obj,':')) {
                    inputs.push_back(std::stoi(obj));
                }
            }
            else if (connections.size() != 0) {
                inputs.push_back(std::stoi(connections));
            }

            digiNets[i] = inputs;
        }

        for(int i = 0; i < nInternalNets; ++i)
        {
            nets[nPins + i] = netSize++;
        }

        setupStates(states);
    }
    void digitalReset(const int* digipins) final
    {
        for(int i = 0; i < nDigipins; ++i)
        {
            //digiNets[i] = digipins[i];
        }
    }
};


struct NetList
{
    typedef std::vector<IComponent*> ComponentList;

    NetList(int nodes, int diginodes) : nets(nodes), states(0), diginets(diginodes)
    {

    }

    void addComponent(IComponent * c)
    {

        c->setupNets(nets, states, c->getPinLocs(), c->getDigiLocs());
        components.push_back(c);
    }

    void buildSystem()
    {
        system.setSize(nets, diginets);
        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->stamp(system);
        }
        printf("Prepare for DC analysis..\n");

        // TODO: doe die begin time skip hier!!!!

        setStepScale((double)1/44100);
        tStep = (double)1/44100;
    }


    void setTimeStep(double tStepSize)
    {
        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->scaleTime(tStep / tStepSize);
        }

        tStep = tStepSize;
        double stepScale = 1. / tStep;
        printf("timeStep changed to %.2g (%.2g Hz)\n", tStep, stepScale);
        setStepScale(stepScale);
    }

    void resetTicks()
    {
        system.ticks = 0;
    }

    double* getAudioOutput() {
        double* output = new double[2];
        for (size_t c = 0; c < 2; c++) {
            for(int i = 0; i < components.size(); ++i)
            {
                output[c] += components[i]->getAudioOutput(system, c);
            }
        }
        return output;

    }

    void setAudioInput(double input) {
        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->setAudioInput(system, input);
        }
    }
    void setMidiInput(std::vector<unsigned char> message) {
        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->setMidiInput(system, message);
        }
    }

    void timeJump(double amt)
    {

        int iter;
        system.time += amt;

        for(iter = 0; iter < maxIter; ++iter)
        {
            // restore matrix state and add dynamic values

            updatePre(tStep);


            if(nets > 1) {
                luFactor();

                luForward();

                luSolve();

                if(newton()) break;
            }
        }

        update();

    }



    void simulateTick()
    {
        //auto t1 = std::chrono::high_resolution_clock::now();
        int iter;

        for(iter = 0; iter < maxIter; ++iter)
        {
            // restore matrix state and add dynamic values

            updatePre(tStep);


            if(nets > 1) {
                luFactor();

                luForward();

                luSolve();

                if(newton()) break;
            }
        }

        system.time += tStep;
        system.ticks++;

        update();
        //auto t2 = std::chrono::high_resolution_clock::now();
        //auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        //std::cout << "Per sample: " << duration << '\n';
    }

    // plotting and such would want to use this
    const MNASystem & getMNA() {
        return system;
    }

protected:
    double  tStep = (double)1/44100;

    int nets, states, diginets;
    ComponentList   components;

    MNASystem       system;

    void update() {


        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->update(system);
        }
        system.digiValues[0] = 0; // node for unconnected digital inlets, clear this to make sure unconnected inlets won't receive eachothers values by accident
        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->updateInput(system);
        }


    }

    // return true if we're done
    bool newton()
    {
        bool done = 1;
        for(int i = 0; i < components.size(); ++i)
        {
            done &= components[i]->newton(system);
        }
        return done;
    }

    void initLU(double stepScale)
    {
        for(int i = 0; i < nets; ++i)
        {
            system.b[i].initLU(stepScale);
            for(int j = 0; j < nets; ++j)
            {
                system.A[i][j].initLU(stepScale);
            }
        }
    }

    void setStepScale(double stepScale)
    {
        // initialize matrix for LU and save it to cache
        initLU(stepScale);

        int fill = 0;
        for(int i = 1; i < nets; ++i)
        {
            for(int j = 1; j < nets; ++j)
            {
                if(system.A[i][j].prelu != 0
                        || system.A[i][j].gdyn.size()) ++fill;
            }
        }
        printf("MNA density %.1f%%\n", 100 * fill / ((nets-1.)*(nets-1.)));
    }

    void updatePre(double stepScale)
    {
        for(int i = 0; i < nets; ++i)
        {
            system.b[i].updatePre(stepScale);
            for(int j = 0; j < nets; ++j)
            {
                system.A[i][j].updatePre(stepScale);
            }
        }
    }

    void luFactor()
    {
        int p;
        for(p = 1; p < nets; ++p)
        {
            // FIND PIVOT
            {
                int pr = p;
                for(int r = p; r < nets; ++r)
                {
                    if(fabs(system.A[r][p].lu)
                            > fabs(system.A[pr][p].lu))
                    {
                        pr = r;
                    }
                }
                // swap if necessary
                if(pr != p)
                {
                    std::swap(system.A[p], system.A[pr]);
                    std::swap(system.b[p], system.b[pr]);
                }
            }
            if(0 == system.A[p][p].lu)
            {
                printf("Failed to find a pivot!!");
                printf("ERROR: Invalid circuit");
                throw;
                return;
            }

            // take reciprocal for D entry
            // NOTE: Is dit de D matrix, (M*M) die standaard 0 is?? zit wel op system a nml
            // hoewel, p,p is een diagonaal...
            system.A[p][p].lu = 1 / system.A[p][p].lu;

            // perform reduction on rows below
            for(int r = p+1; r < nets; ++r)
            {
                if(system.A[r][p].lu == 0) continue;

                system.A[r][p].lu *= system.A[p][p].lu;
                for(int c = p+1; c < nets; ++c)
                {
                    if(system.A[p][c].lu == 0) continue;

                    system.A[r][c].lu -=
                        system.A[p][c].lu * system.A[r][p].lu;
                }

            }
        }
    }

    // this does forward substitution for the solution vector
    int luForward()
    {
        int p;
        for(p = 1; p < nets; ++p)
        {
            // perform reduction on rows below
            // Dit iss mss wel uit te vogelen... welke waardes moeten gesubstitueerd worden?
            for(int r = p+1; r < nets; ++r)
            {
                if(system.A[r][p].lu == 0) continue;
                if(system.b[p].lu == 0) continue;

                system.b[r].lu -= system.b[p].lu * system.A[r][p].lu;
            }
        }
        return p;
    }

    // solves nodes backwards from limit-1
    int luSolve()
    {
        for(int r = nets; --r;)
        {
            //printf("solve node %d\n", r);
            for(int s = r+1; s < nets; ++s)
            {
                system.b[r].lu -= system.b[s].lu * system.A[r][s].lu;
            }

            system.b[r].lu *= system.A[r][r].lu;
        }
        return 1;
    }
};

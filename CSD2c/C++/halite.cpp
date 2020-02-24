#pragma once
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <unistd.h>

#include "AudioFile.h"

#include "MNACell.cpp"
#include "MNASystem.cpp"
#include "IComponent.h"
#include "MNASolver.cpp"
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

constexpr unsigned maxIter = 20;

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

// Stores A and b for A*x - b = 0, where x is the solution.            !!!!!!!!!!!!!!!!!!!

// A en B zijn dus matrix A en Z. Want X is normaal de solution
// Wat krijg je dan eigenlijk met a.lu en b.lu? (bonusvraag waarom is A.lu altijd 1, 0 of -1 wait dat is dan de Z matrix??? maar a.g dan???/)
// A is stored as a vector of rows, for easy in-place pivots
//

// Ik probeer hier om variabelen uit de code te koppelen aan theoretische uitleg van MNA.
// De beste plekken om te beginnen zijn:

// http://qucs.sourceforge.net/tech/node14.html
// https://www.swarthmore.edu/NatSci/echeeve1/Ref/mna/MNA2.html

// In deze documenten wordt gesproken over een A, z en x matrix. Dit heb ik als volgt kunnen thuisbrengen in de code:

// a.g -> conductance (1/weestand) tussen elke node, verder ook aansluiting van voltagebronnen.

// System A bestaat uit 4 matrices die in een matrix zitten, dit zijn de volgende matrices:
// a.g[n][n] komt overeen met A.G
// a.g[m][n] komt overeen met A.B
// a.g[n][m] komt overeen met A.C
// a.g[M][m] komt overeen met A.D

// a.lu -> tussenberekeningen voor pivot????



// b.g -> bekende voltages uit voltagebronnen, vector Z
// b.lu -> oplossingsvector, vector X



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
        solver.setSize(nets, tStep);
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



    void simulateTick()
    {
        //auto t1 = std::chrono::high_resolution_clock::now();

        solver.solve(system);
        //solver.solve2(system);
        //solver.solve3(components, system);

        /*
        std::cout << "B uit:" << '\n';

        for (size_t i = 0; i < aSize; i++) {
          std::cout << system.b[i].lu << '\n';
        }
        usleep(100000); */


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
    MNASystem       system2;
    MNASolver       solver;

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


};

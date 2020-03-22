#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "MNASystem.h"
#include "IComponent.h"



template <int nPins = 0, int nInternalNets = 0, int nDigipins = 0>
struct Component : IComponent
{
    ~Component() {}

    static const int nNets = nPins + nInternalNets;

    int pinLoc[nPins];
    int nets[nNets];
    std::vector<std::vector<int>> digiNets = std::vector<std::vector<int>>(nDigipins);
    std::vector<std::string> digiPins = std::vector<std::string>(nDigipins);

    int pinCount();

    const int* getPinLocs() const;
    const std::vector<std::string> getDigiLocs() const;

    // setup pins and calculate the size of the full netlist
    // the Component<> will handle this automatically
    //
    //  - netSize is the current size of the netlist
    //  - pins is an array of circuits nodes
    //
    void setupNets(int & netSize, int & states, const int* pins, const std::vector<std::string> digiPins);



    // stamp constants into the matrix
    void stamp(MNASystem & m) {};

    // this is for allocating state variables
    void setupStates(int & states) {}

    // update state variables, only tagged nodes
    // this is intended for fixed-time compatible
    // testing to make sure we can code-gen stuff
    void update(MNASystem & m) {}
    void updateInput(MNASystem & m) {}

    // return true if we're done - will keep iterating
    // until all the components are happy
    bool newton(MNASystem & m)
    {
        return true;
    }

    // NEW: output function for probe object;
    double getAudioOutput(MNASystem & m, int c)
    {
        return 0;
    }

    // realtime input function;
    void setAudioInput(MNASystem & m, double input) {}

    void setMidiInput(MNASystem & m, std::vector<unsigned char> &message) {}

    // time-step change, for caps to fix their state-variables
    void scaleTime(double told_per_new) {}
};


#include "Component.cpp" // Linking template classes is hard, this is appearantly the best way, even though it's not optimal

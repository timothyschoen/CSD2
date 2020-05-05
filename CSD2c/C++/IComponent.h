#pragma once
#include "MNASystem.h"

// The reason that we have a separate IComponent is so that we can use the type IComponent to refer to any component without having to specify template arguments
// That would mean that we can't use the class component without specifying how many nets it occupies, which is very inconvenient

struct IComponent
{
    virtual ~IComponent() {}

    // return the number of pins for this component
    virtual int pinCount() {return 0;}


    virtual const int* getPinLocs() const {return 0;}
    virtual const std::vector<std::string> getDigiLocs() const {return std::vector<std::string>(0, 0);}

    // setup pins and calculate the size of the full netlist
    // the Component<> will handle this automatically
    //
    //  - netSize is the current size of the netlist
    //  - pins is an array of circuits nodes
    //
    virtual void setupNets(int & netSize, int & states, const int* pins, const std::vector<std::string> digiPins) {}


    // stamp constants into the matrix
    virtual void stamp(MNASystem & m) {}

    // this is for allocating state variables
    virtual void setupStates(int & states) {}

    // update state variables, only tagged nodes
    // this is intended for fixed-time compatible
    // testing to make sure we can code-gen stuff
    virtual void update(MNASystem & m) {}
    virtual void updateInput(MNASystem & m) {}

    // return true if we're done - will keep iterating
    // until all the components are happy
    virtual bool newton(MNASystem & m)
    {
        return true;
    }

    // NEW: output function for probe object;
    virtual double getAudioOutput(MNASystem & m, int c)
    {
        return 0;
    }

    // realtime input function;
    virtual void setAudioInput(MNASystem & m, double input) {}

    virtual void setMidiInput(MNASystem & m, std::vector<unsigned char> &message) {}

    // time-step change, for caps to fix their state-variables
    virtual void scaleTime(double told_per_new) {}
};

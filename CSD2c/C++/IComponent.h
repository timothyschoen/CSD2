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

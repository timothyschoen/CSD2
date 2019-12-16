#include <unistd.h>
#include <iostream>
#include <cmath>
#pragma once

class Synth
{
public:
    Synth();
    //Both Oscillator and Filter need a function to input a midi pitch which will convert to frequency
    void setPitch(int pitch);
    //Function we can overwrite for the filter, will do nothing on an oscillator subclass
    virtual void calc() {};
    int frequency;
private:
    // Midi To Frequency function
    double mtof(int input);
};

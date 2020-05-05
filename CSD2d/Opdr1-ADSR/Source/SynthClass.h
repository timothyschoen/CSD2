#include <unistd.h>
#include <iostream>
#include <cmath>
#pragma once

class Synth
{
public:
    Synth();
    virtual ~Synth() = 0;
    //Both Oscillator and Filter need a function to input a midi pitch which will convert to frequency
    void setPitch(int pitch);
    void setTranspose(float amount);
    //Function we can overwrite for the filter, will do nothing on an oscillator subclass
    virtual void calc() {};

    float frequency;
    
private:
    double transpose;

    // Midi To Frequency function
    double mtof(int input);

};

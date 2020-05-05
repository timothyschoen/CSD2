#include <unistd.h>
#include <iostream>
#include <cmath>
#pragma once

class Generator
{
public:
    Generator();
    virtual ~Generator() = 0;
    
    //Both Oscillator and Filter need a function to input a midi pitch which will convert to frequency
    void setPitch(int pitch);
    
    void setTranspose(float amount);
    
    //Function we can overwrite for the filter, will do nothing on an oscillator subclass
    virtual void calc() {};
    
    virtual void tick() {};
    
    virtual double getSample(float amplitude);
    

    double output;
    float frequency;
    
private:
    double transpose;

    // Midi To Frequency function
    double mtof(int input);

};

#pragma once

#include <unistd.h>
#include <iostream>
#include <cmath>
#include <thread>
#include "Oscillator.h"
#include "MoogFilter.h"
#include "ADSR.h"



class Synth
{
    Oscillator polyosc1[4] {{44100}, {44100}, {44100}, {44100}};;
    Oscillator polyosc2[4] {{44100}, {44100}, {44100}, {44100}};;
    
    Envelope envelopes[4] {{20, 800, 0.3, 600}, {20, 800, 0.3, 600}, {20, 800, 0.3, 600}, {20, 800, 0.3, 600}};

    Oscillator lfo{1000};

    Envelope modenvelope {20, 800, 0.3, 600};
    // Moog ladder filter
    MoogFilter filt1;
    

    
    int voices = 4;
    int note = 60;
    int freevoice = 3;
    int noteLast[4] = {0, 0, 0, 0};
    
    // Init settings
    double filterpitch = 60;
    float envelopeAmp = 0;
    float lfoAmp = 0;
    
    double* sliderValues;
    
    std::thread controlThread;
    
    public:
    
    Synth();
    ~Synth();
    

    void noteOn(int note);
    
    void noteOff(int note);
    
    double getSample();
    
    void setControl (int index, double value);
    
    void setValuePointer(double* valuepointer);
    
    void controlFunc();
    
    void updateControls();
};

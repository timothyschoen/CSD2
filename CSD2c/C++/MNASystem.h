#pragma once
#include "MNACell.h"
struct MNASystem
{

    typedef std::vector<MNACell>    MNAVector;
    typedef std::vector<MNAVector>  MNAMatrix;

    MNAMatrix   A; // A matrix
    MNAVector   b; // This is the Z matrix since that contains our known values

    // OSC and MIDI input buffer pointers
    std::vector<unsigned char>* midiInput;
    std::vector<double>* oscBuffer;

    double* audioInput; // audio input buffer
    double      time;
    int sampleRate;
    long      ticks; // elapsed ticks
    double      tStep;

    std::vector<double> digiValues;

    void setSize(int n, int d);

    void stampTimed(double g, int r, int c);


    void stampStatic(double g, int r, int c);

    // functions for easier digital manipulation
    void setDigital(std::vector<int> outputs, double value);


    double getDigital(std::vector<int> inputs, double fallback = 0);



};

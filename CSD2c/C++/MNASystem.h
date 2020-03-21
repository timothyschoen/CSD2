#pragma once
#include "MNACell.h"
struct MNASystem
{

    typedef std::vector<MNACell>    MNAVector;
    typedef std::vector<MNAVector>  MNAMatrix;

    MNAMatrix   A; // A matrix
    MNAVector   b; // This is the Z matrix since that contains our known values

    std::vector<unsigned char>* midiInput;
    std::vector<double>* oscBuffer;

    double audioInput[512]; // this is temporary because it's ugly
    double      time;
    long      ticks;
    double      tStep;

    std::vector<double> digiValues;

    void setSize(int n, int d);

    void stampTimed(double g, int r, int c);


    void stampStatic(double g, int r, int c);

    void setDigital(std::vector<int> outputs, double value);


    double getDigital(std::vector<int> inputs, double fallback = 0);



};

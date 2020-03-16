#pragma once
#include <iostream>
#include <vector>


struct MNACell
{
    double g;       // simple values (eg. resistor conductance)
    double gtimed;  // time-scaled values (eg. capacitor conductance)

    // pointers to dynamic variables, added in once per solve
    std::vector<double*>    gdyn;
    std::vector<double*>    gdyntimed;

    double  lu, prelu;  // lu-solver values and matrix pre-LU cache

    void clear();

    void initLU(double stepScale);

    // restore matrix state and update dynamic values
    void updatePre(double stepScale);
};

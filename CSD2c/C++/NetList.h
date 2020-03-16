#pragma once
#include <vector>
#include <iostream>
#include "MNASystem.h"
#include "MNASolver.cpp"
#include "IComponent.h"


struct NetList
{
    typedef std::vector<IComponent*> ComponentList;
    double* output;

    NetList(int nodes, int diginodes);

    void addComponent(IComponent * c);

    void buildSystem();

    void setTimeStep(double tStepSize);

    void resetTicks();

    double* getAudioOutput();

    void setMidiInput(std::vector<unsigned char> &message);

    void setAudioInput(double *buf);


    void simulateTick();

    // plotting and such would want to use this
    const MNASystem & getMNA();

protected:

    double  tStep = (double)1/44100;

    int nets, states, diginets;
    ComponentList   components;

    MNASystem       system;
    MNASolver       solver;


    void update();

    void initLU(double stepScale);

    void setStepScale(double stepScale);

};

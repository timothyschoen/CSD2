#pragma once
#include <vector>
#include <iostream>
#include "MNASystem.h"
#include "MNASolver.h"
#include "IComponent.h"


struct NetList
{
    typedef std::vector<IComponent*> ComponentList;
    double* output;

    NetList(int nodes, int diginodes);

    void addComponent(IComponent * c);

    void buildSystem();

    void setTimeStep(double tStepSize);

    double* getAudioOutput();

    void setMidiInput(std::vector<unsigned char> &message);

    void setOscBuffer(std::vector<double> &oscbuf);


    void setAudioInput(double *buf); // implementeren!!


    void simulateTick();


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

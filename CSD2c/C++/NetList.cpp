#include "NetList.h"


    NetList::NetList(int nodes, int diginodes) : nets(nodes), states(0), diginets(diginodes)
    {
        output = new double[2];
    }

    void NetList::addComponent(IComponent * c)
    {

        c->setupNets(nets, states, c->getPinLocs(), c->getDigiLocs());
        components.push_back(c);
    }

    void NetList::buildSystem()
    {
        system.setSize(nets, diginets);

        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->stamp(system);
        }

        solver.setSize(nets, system);
        setStepScale(0);
        tStep = 0;
    }



    void NetList::setTimeStep(double tStepSize)
    {
        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->scaleTime(tStep / tStepSize);
        }

        tStep = tStepSize;
        double stepScale = 1. / tStep;
        setStepScale(stepScale);

        system.tStep = tStep;
        system.sampleRate = stepScale;
    }


    double* NetList::getAudioOutput()
    {

        for (size_t c = 0; c < 2; c++)
        {
            output[c] = 0;
            for(int i = 0; i < components.size(); ++i)
            {
                output[c] += components[i]->getAudioOutput(system, c);
            }
        }
        return output;

    }

    void NetList::setMidiInput(std::vector<unsigned char> &message)
    {
        system.midiInput = &message;
    }

    void NetList::setOscBuffer(std::vector<double> &oscbuf)
    {
        system.oscBuffer = &oscbuf;
    }

    void NetList::setAudioInput(double *buf)
    {
        //*system.audioInput = buf;
        // FIX THIS
    }

    void NetList::simulateTick()
    {
        //solver.solve3(components, system);
        //auto t1 = std::chrono::high_resolution_clock::now();

        //solver.solve(system);
        //solver.solve2(system);


        //solver.solve3(components, system);
        //auto t2 = std::chrono::high_resolution_clock::now();
        //auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();


        //std::cout << "oude solver: " << duration << "us" << '\n';


        //t1 = std::chrono::high_resolution_clock::now();
        //if(system.ticks < 44100) solver.solve(components, system);


        solver.solveMKL(components, system);

        //solver.solve5(components, system);
        //solver.solve3(components, system);

        //t2 = std::chrono::high_resolution_clock::now();
        //duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

        //std::cout << "nieuwe solver: " << duration << "us" << '\n';
        /*
        std::cout << "B uit:" << '\n';

        for (size_t i = 0; i < aSize; i++) {
          std::cout << system.b[i].lu << '\n';
        }
        usleep(100000); */


        system.time += tStep;
        system.ticks++;


        update();

        //std::cout << "Per sample: " << duration << '\n';
    }



    void NetList::update()
    {
        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->update(system);
        }
        system.digiValues[0] = 0; // node for unconnected digital inlets, clear this to make sure unconnected inlets won't receive eachothers values by accident
        for(int i = 0; i < components.size(); ++i)
        {
            components[i]->updateInput(system);
        }


    }

    void NetList::initLU(double stepScale)
    {
        for(int i = 0; i < nets; ++i)
        {
            system.b[i].initLU(stepScale);
            for(int j = 0; j < nets; ++j)
            {
                system.A[i][j].initLU(stepScale);
            }
        }
    }

    void NetList::setStepScale(double stepScale)
    {
        // initialize matrix for LU and save it to cache
        initLU(stepScale);

        int fill = 0;
        for(int i = 1; i < nets; ++i)
        {
            for(int j = 1; j < nets; ++j)
            {
                if(system.A[i][j].prelu != 0
                        || system.A[i][j].gdyn.size()) ++fill;
            }
        }
    }

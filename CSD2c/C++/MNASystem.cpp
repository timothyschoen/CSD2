#include <iostream>
#include "MNASystem.h"


    void MNASystem::setSize(int n, int d)
    {
        A.resize(n);
        b.resize(n);
        digiValues.resize(d);


        for(unsigned i = 0; i < n; ++i)
        {
            b[i].clear(); // keeping it 21
            A[i].resize(n); // making it 2d


            for(unsigned j = 0; j < n; ++j)
            {
                A[i][j].clear();
            }
        }

        time = 0;
        ticks = 0;
        tStep = 1./44100.; // set to samplerate as default to prevent dividing by 0
    }

    void MNASystem::stampTimed(double g, int r, int c)
    {
        A[r][c].gtimed += g;
    }


    void MNASystem::stampStatic(double g, int r, int c)
    {
        A[r][c].g += g;
    }

    void MNASystem::setDigital(std::vector<int> outputs, double value)
    {
        for (size_t i = 0; i < outputs.size(); i++)
        {
            digiValues[outputs[i]] = value;
        }

    }

    double MNASystem::getDigital(std::vector<int> inputs, double fallback)
    {
        double accum = 0;

            for (size_t i = 0; i < inputs.size(); i++)
            {
                accum += digiValues[inputs[i]];
            }

        accum += fallback*(inputs.size() == 0);

        return accum;
    }

#include <iostream>
#include "MNASystem.h"

    // Set the amount of analog and digital nets
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
        // Set time and ticks to 0
        time = 0;
        ticks = 0;
    }

    // Function for time-scaled
    // Only gets recalculated when the timestep changes (which in our current JS implementation never happens)
    void MNASystem::stampTimed(double g, int r, int c)
    {
        A[r][c].gtimed += g;
    }

    // Stamp a static value (conductance) to the A matrix
    void MNASystem::stampStatic(double g, int r, int c)
    {
        A[r][c].g += g;
    }

    // Helper function for easily setting digital values
    void MNASystem::setDigital(std::vector<int> outputs, double value)
    {
        for (size_t i = 0; i < outputs.size(); i++)
        {
            digiValues[outputs[i]] = value;
        }

    }
    // Helper function for easily getting digital values
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

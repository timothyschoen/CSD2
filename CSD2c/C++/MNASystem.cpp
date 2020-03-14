struct MNASystem
{
    typedef std::vector<MNACell>    MNAVector;
    typedef std::vector<MNAVector>  MNAMatrix;



    MNAMatrix   A; // A matrix
    MNAVector   b; // This is the Z matrix since that contains our known values

    std::vector<unsigned char>* midiInput;
    double audioInput[512];
    double      time;
    long      ticks;
    double      tStep;

    std::vector<double> digiValues;

    void setSize(int n, int d)
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

    void stampTimed(double g, int r, int c)
    {
        A[r][c].gtimed += g;
    }


    void stampStatic(double g, int r, int c)
    {
        A[r][c].g += g;
    }

    void setDigital(std::vector<int> outputs, double value)
    {
        for (size_t i = 0; i < outputs.size(); i++)
        {
            digiValues[outputs[i]] = value;
        }

    }

    double getDigital(std::vector<int> inputs, double fallback = 0)
    {
        double accum = 0;
        if (inputs.size() > 0)
        {
            for (size_t i = 0; i < inputs.size(); i++)
            {
                accum += digiValues[inputs[i]];
            }
        }
        else
        {
            accum = fallback;
        }
        return accum;
    }


};

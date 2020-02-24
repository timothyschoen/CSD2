struct MNACell
{
    double g;       // simple values (eg. resistor conductance)
    double gtimed;  // time-scaled values (eg. capacitor conductance)

    // pointers to dynamic variables, added in once per solve
    std::vector<double*>    gdyn;
    std::vector<double*>    gdyntimed;

    double  lu, prelu;  // lu-solver values and matrix pre-LU cache

    void clear()
    {
        g = 0;
        gtimed = 0;
    }

    void initLU(double stepScale)
    {
        prelu = g + gtimed * stepScale;


    }

    // restore matrix state and update dynamic values
    void updatePre(double stepScale)
    {
        lu = prelu;
        for(int i = 0; i < gdyn.size(); ++i)
        {
            lu += *(gdyn[i]);
        }
        for(int i = 0; i < gdyntimed.size(); ++i)
        {
            lu += *(gdyntimed[i]) * stepScale;
        }
    }
};

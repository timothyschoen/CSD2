
struct Click : Component<2, 1>
{
        double v;
        double amp;

        Click(double amp, int l0, int l1);

        void stamp(MNASystem & m);

        void update(MNASystem & m);

};

// probe a differential voltage
// also forces this voltage to actually get solved :)
struct Probe : Component<2, 1>
{
        float impedance = 1;

        Probe(int l0, int l1);

        void stamp(MNASystem & m);

        double getAudioOutput(MNASystem & m, int channel);

};



struct InputSample : Component<2,1>
{

        double v;
        double amplitude;
        int numSamples;
        int currentSample;
        AudioFile<double> audioFile;

        InputSample(std::string path, double inamp, int l0, int l1);

        void stamp(MNASystem & m);

        void update(MNASystem & m);

};

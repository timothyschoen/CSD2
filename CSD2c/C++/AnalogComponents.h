

struct Resistor : Component<2>
{
    double  r;

    Resistor(double r, int l0, int l1);

    void stamp(MNASystem & m);

};


struct VariableResistor : Component<2, 0, 1>
{
    double  r;
    double  g;
    double  ng;

    VariableResistor(int l0, int l1, std::string d0, std::vector<std::string> init);


    void stamp(MNASystem & m);

    void updateInput(MNASystem & m);


};



struct Potentiometer : Component<3, 0, 1>
{
    double  r;
    double g;
    double ig;
    double input;
    double ng;
    double ing;



    Potentiometer(double r, int l0, int l1, int l2, std::string d0);


    void stamp(MNASystem & m);

    void updateInput(MNASystem & m);


};




struct Capacitor : Component<2, 1>
{
    double c;
    double stateVar;
    double voltage;

    Capacitor(double c, int l0, int l1);


    void stamp(MNASystem & m);

    void update(MNASystem & m);

    void scaleTime(double told_per_new);
};

struct Inductor : Component<2, 1>
{
    double l;
    double g;
    double stateVar;
    double voltage;

    Inductor(double l, int l0, int l1);


    void stamp(MNASystem & m);

    void update(MNASystem & m);


    void scaleTime(double told_per_new);

};

struct Voltage : Component<2, 1>
{
    double v;

    Voltage(double v, int l0, int l1);

    void stamp(MNASystem & m);

};

struct Current : Component<2>
{
    double a;

    Current(double a, int l0, int l1);

    void stamp(MNASystem & m);

};


struct Transformer : Component<4, 2>
{
    double b;

    Transformer(double ratio, int inP, int inN, int outP, int outN);

    void stamp(MNASystem & m);

};


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


// POD-struct for PN-junction data, for diodes and BJTs
//
struct JunctionPN
{
    // variables
    double geq, ieq, veq;

    // parameters
    double is, nvt, rnvt, vcrit;
};

void initJunctionPN(JunctionPN & pn, double is, double n);

void linearizeJunctionPN(JunctionPN & pn, double v);

bool newtonJunctionPN(JunctionPN & pn, double v);


struct Diode : Component<2, 2>
{
    JunctionPN  pn;

    // should make these parameters
    double rs;

    // l0 -->|-- l1 -- parameters default to approx 1N4148
    Diode(int l0, int l1, double rs = 10., double is = 35e-12, double n = 1.24);

    bool newton(MNASystem & m);

    void stamp(MNASystem & m);
};

struct BJT : Component<3, 4>
{
    // emitter and collector junctions
    JunctionPN  pnC, pnE;

    // forward and reverse alpha
    double af, ar, bf, br, rsbc, rsbe;

    int type;

    bool pnp;

    BJT(int b, int c, int e, bool pnp, std::vector<std::string> init);

    bool newton(MNASystem & m);

    void stamp(MNASystem & m);
};


struct OPA : Component<3, 1>
{

    double amp;
    double g;
    double ng;
    double v;
    double vmax;

    OPA(int vInP, int vInN, int vOut);


    void stamp(MNASystem & m);

    void update(MNASystem & m);



};

#include <functional>




// Basic arithmetics
struct digitalArithmetic : Component<0, 0, 3>
{
    double  outvalue;
    double  value;
    std::string name;
    double input;
    std::function<void()> fun;

    digitalArithmetic(std::string type, std::vector<std::string> init, std::string d0, std::string d1, std::string d2);


    void stamp(MNASystem & m);

    void updateInput(MNASystem & m);

    void update(MNASystem & m);
};

struct logarithms : Component<0, 0, 2>
{
    double input;
    std::string name;
    std::function<void()> fun;

    logarithms(std::string type, std::string d0, std::string d1);

    void stamp(MNASystem & m);

    void update(MNASystem & m);

};


struct digitalSignal : Component<0, 0, 1>
{
    double  value;

    digitalSignal(double init, std::string d0);


    void update(MNASystem & m);

};

struct digitalOutput : Component<0, 0, 2>
{
    double value[2];
    float amplitude;

    digitalOutput(float amp, std::string d0, std::string d1);


    void updateInput(MNASystem & m);

    double getAudioOutput(MNASystem & m, int channel);

};


struct digitalAnalogConverter : Component<2,1,1>
{

    double  v;

    digitalAnalogConverter(std::string d0, int l0, int l1);

    void stamp(MNASystem & m);

    void updateInput(MNASystem & m);

};

struct analogDigitalConverter : Component<2, 1, 1>
{
    float impedance = 1;
    double x = 0;
    double y = 0;
    double xm1 = 0;
    double ym1 = 0;
    analogDigitalConverter(int l0, int l1, std::string d0);

    void stamp(MNASystem & m);

    void update(MNASystem & m);

};

struct dcBlock : Component<0, 0, 2>
{
    double x = 0;
    double y = 0;
    double xm1 = 0;
    double ym1 = 0;

    dcBlock(std::string d0, std::string d1);


    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};

struct Changed : Component<0, 0, 2>
{
    double old;
    double in;

    Changed(std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};

struct History : Component<0, 0, 2>
{
    double old;
    double in;

    History(std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};

struct getDelta : Component<0, 0, 2>
{
    double old;
    double in;

    getDelta(std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};


struct accumulate : Component<0, 0, 2>
{
    long double accum;

    accumulate(std::string d0, std::string d1);




    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};


struct absol : Component<0, 0, 2>
{
    double input;

    absol(std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};
struct flor : Component<0, 0, 2>
{
    double input;

    flor(std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);
};

struct ceiling : Component<0, 0, 2>
{
    double input;

    ceiling(std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};

struct Gate : Component<0, 0, 3>
{
    double output;

    Gate(std::string d0, std::string d1, std::string d2);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};

struct Scale : Component<0, 0, 6>
{
    double input;
    double inMin;
    double inMax;
    double outMin;
    double outMax;

    Scale(std::vector<std::string> init, std::string d0, std::string d1, std::string d2, std::string d3, std::string d4, std::string d5);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};

struct Clip : Component<0, 0, 4>
{
    double input;
    double minimum;
    double maximum;


    Clip(std::vector<std::string> init, std::string d0, std::string d1, std::string d2, std::string d3);




    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};

struct Elapsed : Component<0, 0, 1>
{


    Elapsed(std::string d0);



    void update(MNASystem & m);

};




struct digitalInput : Component<0, 0, 1>
{

    double amplitude;
    int numSamples;
    AudioFile<double> audioFile;

    digitalInput(std::string path, double inamp, std::string d0);



    void update(MNASystem & m);

};

struct rtDigitalInput : Component<0, 0, 1>
{

    double amplitude;
    double inputvalue;
    int tick;

    rtDigitalInput(double inamp, std::string d0, int bufsize);



    void update(MNASystem & m);


};




struct midiCtlIn : Component<0, 0, 2>
{

    int cc;
    int value;
    std::vector<unsigned char> copy;

    midiCtlIn(std::string d0, std::string d1);



    void update(MNASystem & m);

    void updateInput(MNASystem & m);

};

struct mToF : Component<0, 0, 2>
{

    double input;

    mToF(std::string d0, std::string d1);


    void update(MNASystem & m);

    void updateInput(MNASystem & m);

};


struct midiNoteIn : Component<0, 0, 2>
{

    int note;
    int velocity;

    std::vector<unsigned char> copy;

    midiNoteIn(std::string d0, std::string d1);



    void update(MNASystem & m);

    void updateInput(MNASystem & m);

};

struct stereoDigitalInput : Component<0,0,2>
{

    double amplitude;
    int numSamples;
    std::string inStrings[2];
    AudioFile<double> audioFile;

    stereoDigitalInput(std::string path, double inamp, std::string d0, std::string d1);



    void update(MNASystem & m);

};

struct slider : Component<0, 0, 1>
{

    float message = 0.5;
    int idx;


    slider(std::string d0, int idx);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);

};


struct sineGenerator : Component<0, 0, 2>
{
    double  freq;
    double phase;

    sineGenerator(std::vector<std::string> a_args, std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);
};

struct squareGenerator : Component<0, 0, 2>
{
    double  freq;
    double phase;
    double sample = 0;

    squareGenerator(std::vector<std::string> a_args, std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);
};

struct triangleGenerator : Component<0, 0, 2>
{
    double  freq;
    double phase;
    double sample = 0;

    triangleGenerator(std::vector<std::string> a_args, std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);
};

struct sawGenerator : Component<0, 0, 2>
{
    double  freq;
    double phase;
    double sample = 0;

    sawGenerator(std::vector<std::string> a_args, std::string d0, std::string d1);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);
};

struct digitalDelay : Component<0, 0, 3>
{
    int t = 10000;
    int bufSize;
    int currentSample;
    double smoothTime;
    int readHead;
    double prevTime;
    std::vector<double> buf;
    float a = 0.00005;

    digitalDelay(std::vector<std::string> init, std::string d0, std::string d1, std::string d2);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);
};

struct digitalPrinter : Component<0, 0, 1>
{
    int perTicks;
    int tick;
    double input;

    digitalPrinter(std::vector<std::string> a_args, std::string d0);



    void updateInput(MNASystem & m);

    void update(MNASystem & m);
};

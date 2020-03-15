#include <functional>
// Digital Components

// Basic arithmetics
struct digitalArithmetic : Component<0, 0, 3>
{
    double  outvalue;
    double  value;
    std::string name;
    double input;
    std::function<void()> fun;

    digitalArithmetic(std::string type, std::vector<std::string> init, std::string d0, std::string d1, std::string d2)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        if (init.size() > 0)
        {
            value = std::stof(init[0]);
        }
        else
        {
            value = 0;
        }
        input = 0;
        name = type;




    }

    void stamp(MNASystem & m) final
    {
        if(!name.compare("+-"))
            fun = [&m, this] (){ m.setDigital(digiNets[2], input+value);};
        else if(!name.compare("--"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input-value);};
        else if(!name.compare("!--"))
            fun = [&m, this] () { m.setDigital(digiNets[2], value-input);};
        else if(!name.compare("*-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input*value);};
        else if(!name.compare("/-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input/value);};
        else if(!name.compare("!/-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], value/input);};
        else if(!name.compare(">-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input>value);};
        else if(!name.compare("<-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input<value);};
        else if(!name.compare(">=-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input>=value);};
        else if(!name.compare("<=-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input<=value);};
        else if(!name.compare("==-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input==value);};
        else if(!name.compare("!=-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input!=value);};
        else if(!name.compare("%-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], fmod(input, value));};
        else if(!name.compare("!%-"))
            fun = [&m, this] () {  m.setDigital(digiNets[2], fmod(value, input));};
        else if(!name.compare("&&-") || !name.compare("and-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input && value);};
        else if(!name.compare("||") || !name.compare("or-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], input || value);};
        else if(!name.compare("sqrt-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], pow(input, 1/value));};
        else if(!name.compare("pow-"))
            fun = [&m, this] () { m.setDigital(digiNets[2], pow(input, value));};
     }

    void updateInput(MNASystem & m) final
    {
        input = m.getDigital(digiNets[0]);
        value = m.getDigital(digiNets[1], value);
    }

    void update(MNASystem & m) final
    {
        fun();

    }
};

struct logarithms : Component<0, 0, 2>
{
    double input;
    std::string name;
    std::function<void()> fun;

    logarithms(std::string type, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        name = type;
    }

    void stamp(MNASystem & m) final
    {
        if(!name.compare("ln-"))
            fun = [&m, this] (){ m.setDigital(digiNets[1], log(input));};
        else if(!name.compare("log2-"))
            fun = [&m, this] () { m.setDigital(digiNets[1], log2(input));};
        else if(!name.compare("log10-"))
            fun = [&m, this] () { m.setDigital(digiNets[1], log10(input));};
    }


    void update(MNASystem & m) final
    {
        fun();
    }
};


struct digitalSignal : Component<0, 0, 1>
{
    double  value;

    digitalSignal(double init, std::string d0)
    {
        digiPins[0] = d0;
        value = init;
    }

    void stamp(MNASystem & m) final
    {  }


    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[0], value);
    }
};

struct digitalOutput : Component<0, 0, 2>
{
    double value[2];
    float amplitude;

    digitalOutput(float amp, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        value[0] = 0;
        value[1] = 0;
        amplitude = amp;
    }

    void stamp(MNASystem & m) final
    {  }


    void updateInput(MNASystem & m) final
    {

        value[0] = m.getDigital(digiNets[0]);
        value[1] = m.getDigital(digiNets[1]);
    }

    double getAudioOutput(MNASystem & m, int channel)
    {
        return value[channel];
    }
};


struct digitalAnalogConverter : Component<2,1,1>
{

    double  v;

    digitalAnalogConverter(std::string d0, int l0, int l1)
    {
        digiPins[0] = d0;
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        v = 0;
    }

    void stamp(MNASystem & m) final
    {
        // this is identical to voltage source
        // except voltage is dynanic
        m.stampStatic(-1, nets[0], nets[2]);
        m.stampStatic(+1, nets[1], nets[2]);

        m.stampStatic(+1, nets[2], nets[0]);
        m.stampStatic(-1, nets[2], nets[1]);

        m.b[nets[2]].gdyn.push_back(&v);

    }

    void updateInput(MNASystem & m) final
    {
        v = m.getDigital(digiNets[0]);
    }

};

struct analogDigitalConverter : Component<2, 1, 1>
{
    float impedance = 1;
    double x = 0;
    double y = 0;
    double xm1 = 0;
    double ym1 = 0;
    analogDigitalConverter(int l0, int l1, std::string d0)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        digiPins[0] = d0;
    }

    void stamp(MNASystem & m) final
    {
        // vp + vn - vd = 0
        m.stampStatic(+impedance, nets[2], nets[0]);
        m.stampStatic(-impedance, nets[2], nets[1]);
        m.stampStatic(-impedance, nets[2], nets[2]);

    }
    void update(MNASystem & m) final
    {
        x = m.b[nets[2]].lu;
        m.setDigital(digiNets[0], x);

    }

};

struct dcBlock : Component<0, 0, 2>
{
    double x = 0;
    double y = 0;
    double xm1 = 0;
    double ym1 = 0;
    dcBlock(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        x = m.getDigital(digiNets[0]);
        y = x - xm1 + 0.995 * ym1;
        xm1 = x;
        ym1 = y;
    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], y);
    }

};

struct Changed : Component<0, 0, 2>
{
    double old;
    double in;

    Changed(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        old = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        in = m.getDigital(digiNets[0]);

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], (in != old));
        in = old;
    }

};

struct History : Component<0, 0, 2>
{
    double old;
    double in;

    History(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        old = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        in = m.getDigital(digiNets[0]);

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], old);
        in = old;
    }

};

struct getDelta : Component<0, 0, 2>
{
    double old;
    double in;

    getDelta(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        old = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        in = m.getDigital(digiNets[0]);

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], in-old);
        in = old;
    }

};


struct accumulate : Component<0, 0, 2>
{
    long double accum;

    accumulate(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        accum = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        accum += m.getDigital(digiNets[0]);

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], accum);
    }

};


struct absol : Component<0, 0, 2>
{
    double input;

    absol(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {

        input = std::abs((int)m.getDigital(digiNets[0]));

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], input);
    }

};
struct flor : Component<0, 0, 2>
{
    double input;

    flor(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        input = m.getDigital(digiNets[0]);

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], int(input));
    }
};

struct ceiling : Component<0, 0, 2>
{
    double input;

    ceiling(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        input = m.getDigital(digiNets[0]);

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], int(input+1));

    }

};

struct Gate : Component<0, 0, 3>
{
    double output;

    Gate(std::string d0, std::string d1, std::string d2)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        output = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        output = (m.getDigital(digiNets[1]) != 0) * m.getDigital(digiNets[0]);

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], output);

    }

};

struct Scale : Component<0, 0, 6>
{
    double input;
    double inMin;
    double inMax;
    double outMin;
    double outMax;

    Scale(std::vector<std::string> init, std::string d0, std::string d1, std::string d2, std::string d3, std::string d4, std::string d5)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        digiPins[3] = d3;
        digiPins[4] = d4;
        digiPins[5] = d5;

        input = 0;
        if(init.size() > 0) inMin = stof(init[0]);
        else inMin = 0;

        if(init.size() > 1) inMax = stof(init[1]);
        else inMax = 0;

        if(init.size() > 2) outMin = stof(init[2]);
        else outMin = 0;

        if(init.size() > 3) outMax = stof(init[3]);
        else outMax = 0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        input = m.getDigital(digiNets[0]);
        inMin = m.getDigital(digiNets[1], inMin);
        inMax = m.getDigital(digiNets[2], inMax);
        outMin = m.getDigital(digiNets[3], outMin);
        outMax = m.getDigital(digiNets[4], outMax);



    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[5], (outMax - outMin) * (input - inMin) / (inMax - inMin) + outMin);

    }

};

struct Clip : Component<0, 0, 4>
{
    double input;
    double minimum;
    double maximum;


    Clip(std::vector<std::string> init, std::string d0, std::string d1, std::string d2, std::string d3)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        digiPins[3] = d3;


        input = 0;
        if(init.size() > 0) minimum = stof(init[0]);
        else minimum = -1;

        if(init.size() > 1) maximum = stof(init[1]);
        else maximum = 1;


    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
        input = m.getDigital(digiNets[0]);
        minimum = m.getDigital(digiNets[1], minimum);
        maximum = m.getDigital(digiNets[2], maximum);




    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[5], fmax(fmin(input, maximum), minimum));

    }

};

struct Elapsed : Component<0, 0, 1>
{


    Elapsed(std::string d0)
    {

        digiPins[0] = d0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[0], (double)m.ticks);

    }

};




struct digitalInput : Component<0, 0, 1>
{

    double amplitude;
    int numSamples;
    AudioFile<double> audioFile;

    digitalInput(std::string path, double inamp, std::string d0)
    {
        digiPins[0] = d0;
        amplitude = inamp;
        audioFile.load(path);
        numSamples = audioFile.getNumSamplesPerChannel();
    }

    void stamp(MNASystem & m) final
    { }

    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[0], audioFile.samples[0][fmod(m.ticks, numSamples)]*amplitude);




    }

};

struct rtDigitalInput : Component<0, 0, 1>
{

    double amplitude;
    double inputvalue;
    int tick;

    rtDigitalInput(double inamp, std::string d0)
    {
        digiPins[0] = d0;
        amplitude = inamp;
        inputvalue = 0;
        tick = 0;

    }

    void stamp(MNASystem & m) final
    {
    }
    void update(MNASystem & m) final
    {
      m.setDigital(digiNets[0], (inbuffer)[tick]);
      tick++;

      if(tick >= 512)  {
        tick = tick - 512;
      }



    }


};




struct midiCtlIn : Component<0, 0, 3>
{

    int cc;
    int value;
    std::vector<unsigned char> copy;

    midiCtlIn(std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        cc = 0;
        value = 0;

    }

    void stamp(MNASystem & m) final
    { }
    void update(MNASystem & m) final
    {
      m.setDigital(digiNets[0], cc);
      m.setDigital(digiNets[1], value);

    }

    void updateInput(MNASystem & m) final
    {
              copy = *m.midiInput;

              if(copy.size() > 0)
              {
                  if ((int)copy[0] == 176) {
                  {
                      cc = (int)copy[1];
                      value = (int)copy[2];
                  }
              }



    }
  }

};

struct mToF : Component<0, 0, 2>
{

    double input;

    mToF(std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;

    }

    void stamp(MNASystem & m) final
    { }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], double(440. * pow(2, ((float)input - 49) / 12)));
    }

    void updateInput(MNASystem & m) final
    {
        input = m.getDigital(digiNets[0]);

    }

};


struct midiNoteIn : Component<0, 0, 2>
{

    int note;
    int velocity;

    std::vector<unsigned char> copy;

    midiNoteIn(std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        note = 0;
        velocity = 0;

    }

    void stamp(MNASystem & m) final
    {

    }
    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[0], note);
        m.setDigital(digiNets[1], velocity);

    }

    void updateInput(MNASystem & m)
    {

        copy = *m.midiInput;

        if(copy.size() > 0)
        {
            if ((int)copy[0] == 144)
            {
                note = (int)copy[1];
                velocity = (int)copy[2];
            }
            else if ((int)copy[0] == 128)
            {
                note = 0;
                velocity = (int)copy[2];
            }
        }
    }

};

struct stereoDigitalInput : Component<0,0,2>
{

    double amplitude;
    int numSamples;
    std::string inStrings[2];
    AudioFile<double> audioFile;

    stereoDigitalInput(std::string path, double inamp, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;

        amplitude = inamp;
        audioFile.load(path);
        numSamples = audioFile.getNumSamplesPerChannel();
    }

    void stamp(MNASystem & m) final
    { }

    void update(MNASystem & m) final
    {

        m.setDigital(digiNets[0], audioFile.samples[0][fmod(m.ticks, numSamples)]*amplitude);
        m.setDigital(digiNets[1], audioFile.samples[1][fmod(m.ticks, numSamples)]*amplitude);

    }

};



struct sineGenerator : Component<0, 0, 2>
{
    double  freq;
    double phase;

    sineGenerator(std::vector<std::string> a_args, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        if(a_args.size() > 0)
            freq = std::stof(a_args[0]);
        else
            freq = 0;

        phase = 0;
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
        freq = m.getDigital(digiNets[0], freq);
        phase += freq / 44100.;
        if(phase >= 1) phase = phase - 1;
    }

    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], (sin(phase * 2. * 3.14159265358979323846)+1.)/2.);
    }
};

struct squareGenerator : Component<0, 0, 2>
{
    double  freq;
    double phase;
    double sample = 0;

    squareGenerator(std::vector<std::string> a_args, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        if(a_args.size() > 0)
            freq = std::stof(a_args[0]);
        else
            freq = 0;

        phase = 0;
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
        freq = m.getDigital(digiNets[0], freq);
        phase += freq / 44100.;
        if(phase >= 1) phase = phase - 1;
    }

    void update(MNASystem & m) final
    {
        if(phase >= 0.5) sample = 1; // 1 and -1 makes it significantly louder than the sine and saw
        else sample = -1;
        m.setDigital(digiNets[1], sample);
    }
};

struct triangleGenerator : Component<0, 0, 2>
{
    double  freq;
    double phase;
    double sample = 0;

    triangleGenerator(std::vector<std::string> a_args, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        if(a_args.size() > 0)
            freq = std::stof(a_args[0]);
        else
            freq = 0;

        phase = 0;
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
        freq = m.getDigital(digiNets[0], freq);
        phase += freq / 44100.;
        if(phase >= 1) phase = phase - 1;
    }

    void update(MNASystem & m) final
    {
        sample = -1.0 + (2.0 * phase);
        sample = 0.9 * (fabs(sample) - 0.5);
        m.setDigital(digiNets[1], sample);
    }
};

struct sawGenerator : Component<0, 0, 2>
{
    double  freq;
    double phase;
    double sample = 0;

    sawGenerator(std::vector<std::string> a_args, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        if(a_args.size() > 0)
            freq = std::stof(a_args[0]);
        else
            freq = 0;

        phase = 0;
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
        freq = m.getDigital(digiNets[0], freq);
        phase += freq / 44100.;
        if(phase >= 1) phase = phase - 1;
    }

    void update(MNASystem & m) final
    {
        m.setDigital(digiNets[1], (1-phase)-0.5);
    }
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

    digitalDelay(std::vector<std::string> init, std::string d0, std::string d1, std::string d2)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;

        int offset = 0; // sometimes a space slips inbetween here, this offset is a fix (might not be necessary anymore?)

        if(init[0].empty()) {
          offset = 1;
        }

        if(init.size() > offset) bufSize = stoi(init[0]);
        else bufSize = 88200;
        if(init.size() > offset+1) t = stof(init[1]);
        else t = 10000;
        currentSample = 0;
        smoothTime = t;
        buf.assign(bufSize, 0);
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
        t = (int)m.getDigital(digiNets[1]);

        if (t>=bufSize) t = bufSize;

        //Write current value
        buf[currentSample] = m.getDigital(digiNets[0]);

    }

    void update(MNASystem & m) final
    {

        currentSample++;

        if (currentSample >= bufSize) currentSample = currentSample-bufSize;
        //if(m.ticks < t) t = 0;

        smoothTime = ((1-a)*smoothTime + a * t);

        if (currentSample-smoothTime < 0) readHead = (int)currentSample-smoothTime+bufSize;
        else readHead = (int)currentSample-smoothTime;

        m.setDigital(digiNets[2], buf[readHead]);


    }
};

struct digitalPrinter : Component<0, 0, 1>
{
    int perTicks;
    int tick;
    double input;

    digitalPrinter(std::vector<std::string> a_args, std::string d0)
    {
        digiPins[0] = d0;
        if(a_args.size() > 0)
            perTicks = std::stoi(a_args[0]);
        else
            perTicks = 20;

    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
        input = m.getDigital(digiNets[0]);
    }

    void update(MNASystem & m) final
    {
      tick++;
      if(tick >= perTicks) {
        std::cout << input << '\n';
        tick = tick - perTicks;
      }
    }
};

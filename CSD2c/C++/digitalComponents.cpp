// Digital Components

// Basic arithmetics
struct digitalArithmetic : Component<0, 0, 3>
{
    double  outvalue;
    double  value;
    std::string name;
    double input;

    digitalArithmetic(std::string type, std::vector<std::string> init, int d0, int d1, int d2)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        if (init.size() > 0) {
         value = std::stof(init[0]);
       }
        input = 0;
        name = type;
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
      input = m.digiValues[digiNets[0]];
      if(digiNets[1])
        value = m.digiValues[digiNets[1]];
    }

    void update(MNASystem & m) final
    {
      if(!name.compare("+-"))
      m.digiValues[digiNets[2]] += input + value;
      else if(!name.compare("--"))
      m.digiValues[digiNets[2]] += input - value;
      else if(!name.compare("!--"))
      m.digiValues[digiNets[2]] += value - input;
      else if(!name.compare("*-"))
      m.digiValues[digiNets[2]] += input * value;
      else if(!name.compare("/-"))
      m.digiValues[digiNets[2]] += input / value;
      else if(!name.compare("!/-"))
      m.digiValues[digiNets[2]] += value / input;
      else if(!name.compare(">-"))
      m.digiValues[digiNets[2]] += input > value;
      else if(!name.compare("<-"))
      m.digiValues[digiNets[2]] += input < value;
      else if(!name.compare(">=-"))
      m.digiValues[digiNets[2]] += input >= value;
      else if(!name.compare("<=-"))
      m.digiValues[digiNets[2]] += input <= value;
      else if(!name.compare("==-"))
      m.digiValues[digiNets[2]] += input += value;
      else if(!name.compare("!=-"))
      m.digiValues[digiNets[2]] += input != value;
      else if(!name.compare("%-"))
      m.digiValues[digiNets[2]] += fmod(input, value);
      else if(!name.compare("!%-"))
      m.digiValues[digiNets[2]] +=fmod(value, input);
      else if(!name.compare("&&-") || !name.compare("and-"))
      m.digiValues[digiNets[2]] += value && input;
      else if(!name.compare("||") || !name.compare("or-"))
      m.digiValues[digiNets[2]] += value || input;
    }
};

struct digitalSignal : Component<0, 0, 2>
{
    double  value;

    digitalSignal(double init, int d0)
    {
        digiPins[0] = d0;
        value = init;
    }

    void stamp(MNASystem & m) final
    {  }


    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[0]] += value;
    }
};

struct digitalOutput : Component<0, 0, 2>
{
    double value[2];
    float amplitude;

    digitalOutput(float amp, int d0, int d1)
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
      value[0] = m.digiValues[digiNets[0]]*amplitude;
      value[1] = m.digiValues[digiNets[1]]*amplitude;
    }

    double getAudioOutput(MNASystem & m, int channel)
    {
      return value[channel];
    }
};


struct digitalAnalogConverter : Component<2,1,1>
{

    double  v;

    digitalAnalogConverter(int d0, int l0, int l1)
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

        m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
    }

    void updateInput(MNASystem & m) final
    {
        v = m.digiValues[digiNets[0]];
    }

};

struct analogDigitalConverter : Component<2, 1, 1>
{
    float impedance = 1;
    double x = 0;
    double y = 0;
    double xm1 = 0;
    double ym1 = 0;
    analogDigitalConverter(int l0, int l1, int d0)
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
      x = m.b[nets[2]].lu * m.nodes[nets[2]].scale;
      y = x - xm1 + 0.995 * ym1;
      xm1 = x;
      ym1 = y;

      m.digiValues[digiNets[0]] += y;
    }

};

struct dcBlock : Component<0, 0, 2>
{
    double x = 0;
    double y = 0;
    double xm1 = 0;
    double ym1 = 0;
    dcBlock(int d0, int d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
    }

    void stamp(MNASystem & m) final
    {

    }

    void updateInput(MNASystem & m) final
    {
      x = m.digiValues[digiNets[0]];
      y = x - xm1 + 0.995 * ym1;
      xm1 = x;
      ym1 = y;
    }
    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[1]] += y;
    }

};

struct Changed : Component<0, 0, 2>
{
    double old;
    double in;

    Changed(int d0, int d1)
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
      in = m.digiValues[digiNets[0]];

    }
    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[1]] += (in != old);
      in = old;
    }

};

struct History : Component<0, 0, 2>
{
    double old;
    double in;

    History(int d0, int d1)
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
      in = m.digiValues[digiNets[0]];

    }
    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[1]] += old;
      in = old;
    }

};

struct getDelta : Component<0, 0, 2>
{
    double old;
    double in;

    getDelta(int d0, int d1)
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

      in = m.digiValues[digiNets[0]];

    }
    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[1]] += in-old;
      in = old;
    }

};

struct Elapsed : Component<0, 0, 1>
{


    Elapsed(int d0, int d1)
    {

        digiPins[0] = d0;
    }

    void stamp(MNASystem & m) final
    {

    }

    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[1]] += m.ticks;
    }

};




struct digitalInput : Component<0, 0, 1>
{

    double amplitude;
    int numSamples;
    AudioFile<double> audioFile;

    digitalInput(std::string path, double inamp, int d0)
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
        if(m.ticks > 0)
          m.digiValues[digiNets[0]] = audioFile.samples[0][fmod(m.ticks, numSamples)]*amplitude;



    }

};

struct rtDigitalInput : Component<0, 0, 1>
{

    double amplitude;
    double inputvalue;

    rtDigitalInput(double inamp, int d0)
    {
        digiPins[0] = d0;
        amplitude = inamp;
        inputvalue = 0;

    }

    void stamp(MNASystem & m) final
    { }
    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[0]] = inputvalue*amplitude;
    }

    void setAudioInput(MNASystem & m, double input) final
    {
        inputvalue = input;



    }

};

struct MidiInput : Component<0, 0, 3>
{

    int input;

    MidiInput(int d0, int d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;

    }

    void stamp(MNASystem & m) final
    { }
    void update(MNASystem & m) final
    {
      //m.digiValues[digiNets[0]] = input;
    }

    void setMidiInput(MNASystem & m, std::vector<unsigned char> message) final
    {
        //input = message;



    }

};

struct mToF : Component<0, 0, 2>
{

    double input;

    mToF(int d0, int d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;

    }

    void stamp(MNASystem & m) final
    { }
    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[1]] = double(440. * pow(2, ((float)input - 49) / 12));
    }

    void updateInput(MNASystem & m) final
    {
        input = m.digiValues[digiNets[0]];

    }

};


struct midiNoteIn : Component<0, 0, 2>
{

    int note;
    int velocity;

    midiNoteIn(int d0, int d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        note = 0;
        velocity = 0;

    }

    void stamp(MNASystem & m) final
    { }
    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[0]] = note;
      m.digiValues[digiNets[1]] = velocity;
    }

    void setMidiInput(MNASystem & m, std::vector<unsigned char> message) final
    {
      if ((int)message[0] == 144 && (int)message[2] != 0) {
          note = (int)message[1];
          velocity = (int)message[2];
      }
      else if ((int)message[0] == 128 || ((int)message[0] == 144 && (int)message[2] != 0)) {
        note = 0;
        velocity = 0;
      }
    }

};

struct stereoDigitalInput : Component<0,0,2>
{

    double amplitude;
    int numSamples;
    AudioFile<double> audioFile;

    stereoDigitalInput(std::string path, double inamp, int d0, int d1)
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
        if(m.ticks > 0)
          m.digiValues[digiNets[0]] = audioFile.samples[0][fmod(m.ticks, numSamples)]*amplitude;
          m.digiValues[digiNets[1]] = audioFile.samples[1][fmod(m.ticks, numSamples)]*amplitude;
    }

};



struct digitalCycle : Component<0, 0, 2>
{
    double  freq;
    double phase;

    digitalCycle(std::vector<std::string> a_args, int d0, int d1)
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
      if(digiNets[0]) freq = m.digiValues[digiNets[0]];
      phase += freq / 44100.;
      if(phase >= 1) phase = phase - 1;
    }

    void update(MNASystem & m) final
    {
      m.digiValues[digiNets[1]] += (sin(phase * 2. * 3.14159265358979323846)+1.)/2.;
    }
};

struct digitalDelay : Component<0, 0, 3>
{
    int t = 10000;
    int currentSample;
    double smoothTime;
    double prevTime;
    std::vector<double> buf;
    float a = 0.05;

    digitalDelay(std::vector<std::string> init, int d0, int d1, int d2)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        if(init.size() > 0) buf.resize(stoi(init[0]));
        else buf.resize(88200);
        if(init.size() > 1) t = stof(init[1]);
        else t = 10000;
        currentSample = 0;
        smoothTime = t;
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
    if(digiNets[1]) t = (int)m.digiValues[digiNets[1]];

    //Write current value
    buf[currentSample] = m.digiValues[digiNets[0]];
    }

    void update(MNASystem & m) final
    {

      currentSample++;
      // Don't delay while loading
      if (currentSample >= 88200) currentSample = currentSample-88200;



      smoothTime = (1-a)*prevTime + a * t;
      prevTime = smoothTime;

      // Read buffered value
      m.digiValues[digiNets[2]] += buf[int(currentSample+88200-smoothTime)%88200];


    }
};

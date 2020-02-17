// Digital Components

// Basic arithmetics
struct digitalArithmetic : Component<0, 0, 3>
{
    double  outvalue;
    double  value;
    std::string name;
    double input;

    digitalArithmetic(std::string type, double init, int d0, int d1, int d2)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        value = init;
        input = 0;
        name = type;
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
      input = m.digiValues[digiNets[0]];
      if(digiNets[1]) {
        value = m.digiValues[digiNets[1]];
      }
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

    double getOutput(MNASystem & m, int channel)
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
      m.digiValues[digiNets[0]] += m.b[nets[2]].lu * m.nodes[nets[2]].scale;
    }

};



struct digitalInput : Component<0, 0, 1>
{

    double v;
    double amplitude;
    int numSamples;
    AudioFile<double> audioFile;

    digitalInput(std::string path, double inamp, int d0)
    {
        digiPins[0] = d0;
        amplitude = inamp;
        audioFile.load(path);
        numSamples = audioFile.getNumSamplesPerChannel();
        v = 0;
    }

    void stamp(MNASystem & m) final
    { }

    void update(MNASystem & m) final
    {
        if(m.ticks > 0)
          m.digiValues[digiNets[0]] = audioFile.samples[0][fmod(m.ticks, numSamples)]*amplitude;



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

    digitalCycle(double hz, int d0, int d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        freq = hz;
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
    double buf[88200];
    float a = 0.05;

    digitalDelay(int d0, int d1, int d2)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        t = 10000;
        currentSample = 0;
        smoothTime = t;
    }

    void stamp(MNASystem & m) final
    {  }

    void updateInput(MNASystem & m) final
    {
      t = (int)m.digiValues[digiNets[1]];

    //Write current value
    buf[currentSample] = m.digiValues[digiNets[0]];
    }

    void update(MNASystem & m) final
    {

      // Don't delay while loading
      currentSample = m.ticks%88200;
      if(m.ticks < 0) currentSample = 0;


      smoothTime = (1-a)*prevTime + a * t;
      prevTime = smoothTime;


      // Read buffered value
      m.digiValues[digiNets[2]] += buf[int(currentSample+88200-smoothTime)%88200];


    }
};

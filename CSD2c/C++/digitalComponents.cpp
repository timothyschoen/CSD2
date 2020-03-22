#include <functional>
#include "AudioFile.h"
#include "Component.h"
#include "digitalComponents.h"

// Digital Components


    digitalArithmetic::digitalArithmetic(std::string type, std::vector<std::string> init, std::string d0, std::string d1, std::string d2)
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

    void digitalArithmetic::stamp(MNASystem & m)
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

    void digitalArithmetic::updateInput(MNASystem & m)
    {
        input = m.getDigital(digiNets[0]);
        value = m.getDigital(digiNets[1], value);
    }

    void digitalArithmetic::update(MNASystem & m)
    {
        fun();

    }


    logarithms::logarithms(std::string type, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        name = type;
    }

    void logarithms::stamp(MNASystem & m)
    {
        if(!name.compare("ln-"))
            fun = [&m, this] (){ m.setDigital(digiNets[1], log(input));};
        else if(!name.compare("log2-"))
            fun = [&m, this] () { m.setDigital(digiNets[1], log2(input));};
        else if(!name.compare("log10-"))
            fun = [&m, this] () { m.setDigital(digiNets[1], log10(input));};
    }


    void logarithms::update(MNASystem & m)
    {
        fun();
    }


    digitalSignal::digitalSignal(double init, std::string d0)
    {
        digiPins[0] = d0;
        value = init;
    }



    void digitalSignal::update(MNASystem & m)
    {
        m.setDigital(digiNets[0], value);
    }


    digitalOutput::digitalOutput(float amp, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        value[0] = 0;
        value[1] = 0;
        amplitude = amp;
    }


    void digitalOutput::updateInput(MNASystem & m)
    {

        value[0] = m.getDigital(digiNets[0]);
        value[1] = m.getDigital(digiNets[1]);
    }

    double digitalOutput::getAudioOutput(MNASystem & m, int channel)
    {
        return value[channel];
    }


    digitalAnalogConverter::digitalAnalogConverter(std::string d0, int l0, int l1)
    {
        digiPins[0] = d0;
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        v = 0;
    }

    void digitalAnalogConverter::stamp(MNASystem & m)
    {
        // this is identical to voltage source
        // except voltage is dynanic
        m.stampStatic(-1, nets[0], nets[2]);
        m.stampStatic(+1, nets[1], nets[2]);

        m.stampStatic(+1, nets[2], nets[0]);
        m.stampStatic(-1, nets[2], nets[1]);

        m.b[nets[2]].gdyn.push_back(&v);

    }

    void digitalAnalogConverter::updateInput(MNASystem & m)
    {
        v = m.getDigital(digiNets[0]);
    }



    analogDigitalConverter::analogDigitalConverter(int l0, int l1, std::string d0)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        digiPins[0] = d0;
    }

    void analogDigitalConverter::stamp(MNASystem & m)
    {
        // vp + vn - vd = 0
        m.stampStatic(+impedance, nets[2], nets[0]);
        m.stampStatic(-impedance, nets[2], nets[1]);
        m.stampStatic(-impedance, nets[2], nets[2]);

    }
    void analogDigitalConverter::update(MNASystem & m)
    {
        x = m.b[nets[2]].lu;
        m.setDigital(digiNets[0], x);

    }


    dcBlock::dcBlock(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
    }

    void dcBlock::updateInput(MNASystem & m)
    {
        x = m.getDigital(digiNets[0]);
        y = x - xm1 + 0.995 * ym1;
        xm1 = x;
        ym1 = y;
    }
    void dcBlock::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], y);
    }


    Changed::Changed(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        old = 0;
    }



    void Changed::updateInput(MNASystem & m)
    {
        in = m.getDigital(digiNets[0]);

    }
    void Changed::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], (in != old));
        in = old;
    }


    History::History(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        old = 0;
    }


    void History::updateInput(MNASystem & m)
    {
        in = m.getDigital(digiNets[0]);

    }
    void History::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], old);
        in = old;
    }



    getDelta::getDelta(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        old = 0;
    }


    void getDelta::updateInput(MNASystem & m)
    {
        in = m.getDigital(digiNets[0]);

    }
    void getDelta::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], in-old);
        in = old;
    }


    accumulate::accumulate(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        accum = 0;
    }


    void accumulate::updateInput(MNASystem & m)
    {
        accum += m.getDigital(digiNets[0]);

    }
    void accumulate::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], accum);
    }


    absol::absol(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;
    }


    void absol::updateInput(MNASystem & m)
    {

        input = std::abs((int)m.getDigital(digiNets[0]));

    }
    void absol::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], input);
    }



    flor::flor(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;
    }


    void flor::updateInput(MNASystem & m)
    {
        input = m.getDigital(digiNets[0]);

    }
    void flor::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], int(input));
    }




    ceiling::ceiling(std::string d0, std::string d1)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;
    }


    void ceiling::updateInput(MNASystem & m)
    {
        input = m.getDigital(digiNets[0]);

    }
    void ceiling::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], int(input+1));

    }



    Gate::Gate(std::string d0, std::string d1, std::string d2)
    {

        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;
        output = 0;
    }



    void Gate::updateInput(MNASystem & m)
    {
        output = (m.getDigital(digiNets[1]) != 0) * m.getDigital(digiNets[0]);

    }
    void Gate::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], output);

    }



    Scale::Scale(std::vector<std::string> init, std::string d0, std::string d1, std::string d2, std::string d3, std::string d4, std::string d5)
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

    void Scale::updateInput(MNASystem & m)
    {
        input = m.getDigital(digiNets[0]);
        inMin = m.getDigital(digiNets[1], inMin);
        inMax = m.getDigital(digiNets[2], inMax);
        outMin = m.getDigital(digiNets[3], outMin);
        outMax = m.getDigital(digiNets[4], outMax);



    }
    void Scale::update(MNASystem & m)
    {
        m.setDigital(digiNets[5], (outMax - outMin) * (input - inMin) / (inMax - inMin) + outMin);

    }


    Clip::Clip(std::vector<std::string> init, std::string d0, std::string d1, std::string d2, std::string d3)
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



    void Clip::updateInput(MNASystem & m)
    {
        input = m.getDigital(digiNets[0]);
        minimum = m.getDigital(digiNets[1], minimum);
        maximum = m.getDigital(digiNets[2], maximum);




    }
    void Clip::update(MNASystem & m)
    {
        m.setDigital(digiNets[5], fmax(fmin(input, maximum), minimum));

    }




    Elapsed::Elapsed(std::string d0)
    {
        digiPins[0] = d0;
    }


    void Elapsed::update(MNASystem & m)
    {
        m.setDigital(digiNets[0], (double)m.ticks);

    }



    digitalInput::digitalInput(std::string path, double inamp, std::string d0)
    {
        digiPins[0] = d0;
        amplitude = inamp;
        audioFile.load(path);
        numSamples = audioFile.getNumSamplesPerChannel();
    }

    void digitalInput::update(MNASystem & m)
    {
        m.setDigital(digiNets[0], audioFile.samples[0][fmod(m.ticks, numSamples)]*amplitude);

    }


    rtDigitalInput::rtDigitalInput(double inamp, std::string d0, int bufsize)
    {
        digiPins[0] = d0;
        amplitude = inamp;
        inputvalue = 0;
        tick = 0;

    }


    void rtDigitalInput::update(MNASystem & m)
    {
      // FIX REALTIME INPUT
      //m.setDigital(digiNets[0], (inbuffer)[tick]);
      tick++;
      tick = tick & 511;

    }




    midiCtlIn::midiCtlIn(std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        cc = 0;
        value = 0;
    }


    void midiCtlIn::update(MNASystem & m)
    {
      m.setDigital(digiNets[0], cc);
      m.setDigital(digiNets[1], value);

    }

    void midiCtlIn::updateInput(MNASystem & m)
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


    mToF::mToF(std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        input = 0;

    }


    void mToF::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], double(440. * pow(2, ((float)input - 49) / 12)));
    }

    void mToF::updateInput(MNASystem & m)
    {
        input = m.getDigital(digiNets[0]);

    }


    midiNoteIn::midiNoteIn(std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        note = 0;
        velocity = 0;

    }


    void midiNoteIn::update(MNASystem & m)
    {
        m.setDigital(digiNets[0], note);
        m.setDigital(digiNets[1], velocity);

    }

    void midiNoteIn::updateInput(MNASystem & m)
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


    stereoDigitalInput::stereoDigitalInput(std::string path, double inamp, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;

        amplitude = inamp;
        audioFile.load(path);
        numSamples = audioFile.getNumSamplesPerChannel();
    }



    void stereoDigitalInput::update(MNASystem & m)
    {

        m.setDigital(digiNets[0], audioFile.samples[0][fmod(m.ticks, numSamples)]*amplitude);
        m.setDigital(digiNets[1], audioFile.samples[1][fmod(m.ticks, numSamples)]*amplitude);

    }



    slider::slider(std::string d0, int idx) : idx(idx)
    {
        digiPins[0] = d0;
    }

    void slider::updateInput(MNASystem & m)
    {

      message = (*m.oscBuffer)[idx];
    }

    void slider::update(MNASystem & m)
    {
        m.setDigital(digiNets[0], message);
    }


    sineGenerator::sineGenerator(std::vector<std::string> a_args, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        if(a_args.size() > 0)
            freq = std::stof(a_args[0]);
        else
            freq = 0;

        phase = 0;
    }


    void sineGenerator::updateInput(MNASystem & m)
    {
        freq = m.getDigital(digiNets[0], freq);
        phase += freq / m.sampleRate;
        if(phase >= 1) phase = phase - 1;
    }

    void sineGenerator::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], (sin(phase * 2. * 3.14159265358979323846)+1.)/2.);
    }


    squareGenerator::squareGenerator(std::vector<std::string> a_args, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        if(a_args.size() > 0)
            freq = std::stof(a_args[0]);
        else
            freq = 0;

        phase = 0;
    }


    void squareGenerator::updateInput(MNASystem & m)
    {
        freq = m.getDigital(digiNets[0], freq);
        phase += freq / m.sampleRate;
        if(phase >= 1) phase = phase - 1;
    }

    void squareGenerator::update(MNASystem & m)
    {
        if(phase >= 0.5) sample = 1; // 1 and -1 makes it significantly louder than the sine and saw
        else sample = -1;
        m.setDigital(digiNets[1], sample);
    }

    triangleGenerator::triangleGenerator(std::vector<std::string> a_args, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        if(a_args.size() > 0)
            freq = std::stof(a_args[0]);
        else
            freq = 0;

        phase = 0;
    }


    void triangleGenerator::updateInput(MNASystem & m)
    {
        freq = m.getDigital(digiNets[0], freq);
        phase += freq / m.sampleRate;
        if(phase >= 1) phase = phase - 1;
    }

    void triangleGenerator::update(MNASystem & m)
    {
        sample = -1.0 + (2.0 * phase);
        sample = 0.9 * (fabs(sample) - 0.5);
        m.setDigital(digiNets[1], sample);
    }


    sawGenerator::sawGenerator(std::vector<std::string> a_args, std::string d0, std::string d1)
    {
        digiPins[0] = d0;
        digiPins[1] = d1;
        if(a_args.size() > 0)
            freq = std::stof(a_args[0]);
        else
            freq = 0;

        phase = 0;
    }


    void sawGenerator::updateInput(MNASystem & m)
    {
        freq = m.getDigital(digiNets[0], freq);
        phase += freq / m.sampleRate;
        if(phase >= 1) phase = phase - 1;
    }

    void sawGenerator::update(MNASystem & m)
    {
        m.setDigital(digiNets[1], (1-phase)-0.5);
    }


    digitalDelay::digitalDelay(std::vector<std::string> init, std::string d0, std::string d1, std::string d2)
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

        bufSize = pow(2, ceil(log(bufSize)/log(2)))-1; //prepare for bitwise AND

        buf.assign(bufSize, 0);
    }


    void digitalDelay::updateInput(MNASystem & m)
    {
        t = (int)m.getDigital(digiNets[1]) + (t>=bufSize)*bufSize; // trying to avoid if statements

        //Write current value
        buf[currentSample] = m.getDigital(digiNets[0]);

    }

    void digitalDelay::update(MNASystem & m)
    {

        currentSample++;
        currentSample = currentSample & bufSize; // Bitwise AND, faster than modulo or if-statements

        smoothTime = ((1-a)*smoothTime + a * t); // smoothing readhead movement

        readHead = (int)currentSample-smoothTime;
        readHead += (readHead<=0)*bufSize;

        m.setDigital(digiNets[2], buf[readHead]);


    }


    digitalPrinter::digitalPrinter(std::vector<std::string> a_args, std::string d0)
    {
        digiPins[0] = d0;
        if(a_args.size() > 0)
            perTicks = std::stoi(a_args[0]);
        else
            perTicks = 22050;

    }

    void digitalPrinter::updateInput(MNASystem & m)
    {
        input = m.getDigital(digiNets[0]);
    }

    void digitalPrinter::update(MNASystem & m)
    {
      tick++;
      if(tick >= perTicks) {
        std::cout << input << '\n';
        tick = tick - perTicks;
      }
    }

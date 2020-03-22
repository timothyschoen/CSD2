#include "../AudioFile.h"
#include "../Component.h"
#include "digitalIO.h"



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

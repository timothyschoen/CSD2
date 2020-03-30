#include "../AudioFile.h"
#include "../Component.h"
#include "analogIO.h"
//
//                  CLICK
//

Click::Click(double amp, int l0, int l1) : amp(amp)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;
}

void Click::stamp(MNASystem & m)
{
        // Gets written to A matrix (B and C parts)
        m.stampStatic(-1, nets[0], nets[2]); // -1 to the net connected to the negative
        m.stampStatic(+1, nets[1], nets[2]);

        m.stampStatic(+1, nets[2], nets[0]);
        m.stampStatic(-1, nets[2], nets[1]);

        m.b[nets[2]].gdyn.push_back(&v);

}
void Click::update(MNASystem & m)
{

        v = (m.ticks == 1)*amp;

}

//
//                  PROBE
//

Probe::Probe(int l0, int l1)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;
}

void Probe::stamp(MNASystem & m)
{
        m.stampStatic(+1, nets[2], nets[0]);
        m.stampStatic(-1, nets[2], nets[1]);
        m.stampStatic(-1, nets[2], nets[2]);

}

double Probe::getAudioOutput(MNASystem & m, int channel)
{

        return m.b[nets[2]].lu;

}

//
//                  SAMPLE INPUT
//


InputSample::InputSample(std::string path, double inamp, int l0, int l1)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        amplitude = inamp;
        audioFile.load(path);
        numSamples = audioFile.getNumSamplesPerChannel();
        currentSample = 0;
        v = 0;
}

void InputSample::stamp(MNASystem & m)
{
        // this is identical to voltage source
        // except voltage is dynanic
        m.stampStatic(-0.1, nets[0], nets[2]);
        m.stampStatic(+0.1, nets[1], nets[2]);

        m.stampStatic(+0.1, nets[2], nets[0]);
        m.stampStatic(-0.1, nets[2], nets[1]);

        m.b[nets[2]].gdyn.push_back(&v);

}

void InputSample::update(MNASystem & m)
{
        currentSample++;
        if (currentSample >= numSamples) currentSample = currentSample - numSamples;

        v = audioFile.samples[0][currentSample]*amplitude;

}

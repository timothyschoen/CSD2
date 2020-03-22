#include <math.h>
#include "../Component.h"
#include "digitalSignalGenerators.h"


digitalSignal::digitalSignal(double init, std::string d0)
{
        digiPins[0] = d0;
        value = init;
}



void digitalSignal::update(MNASystem & m)
{
        m.setDigital(digiNets[0], value);
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

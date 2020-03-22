#include <math.h>
#include <functional>
#include "../Component.h"
#include "digitalUtility.h"

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

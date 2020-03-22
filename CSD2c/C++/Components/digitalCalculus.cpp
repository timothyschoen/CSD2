#include <functional>
#include <math.h>
#include "../Component.h"
#include "digitalCalculus.h"


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
                fun = [&m, this] (){
                              m.setDigital(digiNets[2], input+value);
                      };
        else if(!name.compare("--"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input-value);
                      };
        else if(!name.compare("!--"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], value-input);
                      };
        else if(!name.compare("*-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input*value);
                      };
        else if(!name.compare("/-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input/value);
                      };
        else if(!name.compare("!/-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], value/input);
                      };
        else if(!name.compare(">-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input>value);
                      };
        else if(!name.compare("<-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input<value);
                      };
        else if(!name.compare(">=-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input>=value);
                      };
        else if(!name.compare("<=-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input<=value);
                      };
        else if(!name.compare("==-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input==value);
                      };
        else if(!name.compare("!=-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input!=value);
                      };
        else if(!name.compare("%-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], fmod(input, value));
                      };
        else if(!name.compare("!%-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], fmod(value, input));
                      };
        else if(!name.compare("&&-") || !name.compare("and-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input && value);
                      };
        else if(!name.compare("||") || !name.compare("or-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], input || value);
                      };
        else if(!name.compare("sqrt-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], pow(input, 1/value));
                      };
        else if(!name.compare("pow-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[2], pow(input, value));
                      };
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
                fun = [&m, this] (){
                              m.setDigital(digiNets[1], log(input));
                      };
        else if(!name.compare("log2-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[1], log2(input));
                      };
        else if(!name.compare("log10-"))
                fun = [&m, this] () {
                              m.setDigital(digiNets[1], log10(input));
                      };
}


void logarithms::update(MNASystem & m)
{
        fun();
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

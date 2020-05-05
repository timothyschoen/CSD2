#include <math.h>
#include "../Component.h"
#include "analogInteraction.h"
//
//                VARIABLE RESISTOR
//

VariableResistor::VariableResistor(int l0, int l1, std::string d0, std::vector<std::string> init)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        digiPins[0] = d0;
        if (init.size() > 0)
        {
                r = std::stod(init[0]);
        }
        else
        {
                r = 10000;
        }
        g = 1. / r;
        ng = -g;


}

void VariableResistor::stamp(MNASystem & m)
{


        m.A[nets[0]][nets[0]].gdyn.push_back(&g);
        m.A[nets[0]][nets[1]].gdyn.push_back(&ng);
        m.A[nets[1]][nets[0]].gdyn.push_back(&ng);
        m.A[nets[1]][nets[1]].gdyn.push_back(&g);

}
void VariableResistor::updateInput(MNASystem & m)
{
        r = m.getDigital(digiNets[0], 1);
        r += r == 0; // cant have 0

        g = 1. / r;
        ng = -g;

}


//
//                POTENTIOMETER
//

Potentiometer::Potentiometer(double r, int l0, int l1, int l2, std::string d0) : r(r)
{
        pinLoc[0] = l0; // in
        pinLoc[1] = l1; // out
        pinLoc[2] = l2; // inv out
        digiPins[0] = d0; // set value

        g = r*0.5;
        ig = r*0.5;

        ng = -g;
        ing = -ig;
}

void Potentiometer::stamp(MNASystem & m)
{

        m.A[nets[0]][nets[0]].gdyn.push_back(&g);
        m.A[nets[0]][nets[1]].gdyn.push_back(&ng);
        m.A[nets[1]][nets[0]].gdyn.push_back(&ng);
        m.A[nets[1]][nets[1]].gdyn.push_back(&g);

        m.A[nets[0]][nets[0]].gdyn.push_back(&ig);
        m.A[nets[0]][nets[2]].gdyn.push_back(&ing);
        m.A[nets[2]][nets[0]].gdyn.push_back(&ing);
        m.A[nets[2]][nets[2]].gdyn.push_back(&ig);

}
void Potentiometer::updateInput(MNASystem & m)
{
        input =  m.getDigital(digiNets[0], 0.5);
        input = fmax(fmin(input, 0.9), 0.1); // take out the extremes and prevent 0 divides

        g = 1. / (r * input);
        ig = 1. / (r - (r * input));

        ng = -g;
        ing = -ig;


}

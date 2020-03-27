#include <math.h>
#include "../Component.h"
#include "analogBasics.h"

//
//                  RESISTOR
//

Resistor::Resistor(double r, int l0, int l1) : r(r)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;
}

void Resistor::stamp(MNASystem & m)
{

        double g = 1. / r;
        m.stampStatic(+g, nets[0], nets[0]); //Positive on diagonal elements
        m.stampStatic(-g, nets[0], nets[1]); //Negative on off-diagonal elements
        m.stampStatic(-g, nets[1], nets[0]);
        m.stampStatic(+g, nets[1], nets[1]);
}


//
//                  CAPACITOR
//

Capacitor::Capacitor(double c, int l0, int l1) : c(c)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;

        stateVar = 0;
        voltage = 0;
}

void Capacitor::stamp(MNASystem & m)
{


        double g = 2*c;


        m.stampTimed(+1, nets[0], nets[2]); // Naar A->B system
        m.stampTimed(-1, nets[1], nets[2]);

        // Weerstand parallel aan spanningsbron
        m.stampTimed(-g, nets[0], nets[0]);
        m.stampTimed(+g, nets[0], nets[1]);
        m.stampTimed(+g, nets[1], nets[0]);
        m.stampTimed(-g, nets[1], nets[1]);

        m.stampStatic(+2*g, nets[2], nets[0]);  // Naar A->C system
        m.stampStatic(-2*g, nets[2], nets[1]);


        m.stampStatic(-1, nets[2], nets[2]); // Naar A->D system

        // see the comment about v:C[%d] below
        m.b[nets[2]].gdyn.push_back(&stateVar);


}

void Capacitor::update(MNASystem & m)
{


        stateVar = m.b[nets[2]].lu; // t - h

        // solve legit voltage from the pins
        voltage = m.b[nets[0]].lu - m.b[nets[1]].lu; // t

}

void Capacitor::scaleTime(double told_per_new)
{

        double qq = 2*c*voltage;
        stateVar = qq + (stateVar - qq)*told_per_new;
}


//
//                  INDUCTOR
//

Inductor::Inductor(double l, int l0, int l1) : l(l)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;

        stateVar = 0;
        voltage = 0;
}

void Inductor::stamp(MNASystem & m)
{




        g = 1/((2.*l)/m.sampleRate);

        m.stampStatic(+1, nets[0], nets[2]); // Naar A->B system
        m.stampStatic(-1, nets[1], nets[2]);

        m.stampStatic(-1, nets[2], nets[0]);  // Naar A->C system
        m.stampStatic(+1, nets[2], nets[1]);

        m.A[nets[2]][nets[2]].gdyn.push_back(&g);


        m.b[nets[2]].gdyn.push_back(&stateVar);

}

void Inductor::update(MNASystem & m)
{

        g = 1/((2.*l)/m.sampleRate);


        stateVar = voltage + g * m.b[nets[2]].lu;

        // solve legit voltage from the pins
        voltage = (m.b[nets[0]].lu - m.b[nets[1]].lu);

}

void Inductor::scaleTime(double told_per_new)
{

}

//
//                  VOLTAGE
//


Voltage::Voltage(double v, int l0, int l1) : v(v)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;
}

void Voltage::stamp(MNASystem & m)
{
        // Gets written to A matrix (B and C parts)
        m.stampStatic(-1, nets[0], nets[2]); // -1 to the net connected to the negative
        m.stampStatic(+1, nets[1], nets[2]);

        m.stampStatic(+1, nets[2], nets[0]);
        m.stampStatic(-1, nets[2], nets[1]);

        m.b[nets[2]].g = v;

}

//
//                  CURRENT SOURCE
//

Current::Current(double a, int l0, int l1) : a(a)
{
        pinLoc[0] = l0;
        pinLoc[1] = l1;
}

void Current::stamp(MNASystem & m)
{

        m.b[nets[0]].g = -a;
        m.b[nets[1]].g = a;

}

//
//                  TRANSFORMER
//

Transformer::Transformer(double ratio, int inP, int inN, int outP, int outN) : b(ratio)
{
        pinLoc[0] = inN;
        pinLoc[1] = inP;
        pinLoc[2] = outN;
        pinLoc[3] = outP;
}

void Transformer::stamp(MNASystem & m)
{


        m.stampStatic(1, nets[5], nets[3]);
        m.stampStatic(-1, nets[5], nets[4]);
        m.stampStatic(1, nets[4], nets[4]);

        m.stampStatic(1, nets[0], nets[4]);
        m.stampStatic(-1, nets[1], nets[4]);
        m.stampStatic(1, nets[2], nets[5]);
        m.stampStatic(-1, nets[3], nets[5]);

        m.stampStatic(-b, nets[5], nets[0]);
        m.stampStatic(b, nets[5], nets[1]);
        m.stampStatic(-b, nets[4], nets[5]);

}

//
//                  OPAMP
//




// pins: out, in+, in-, supply+, supply-
OPA::OPA(int vInP, int vInN, int vOut)
{
        pinLoc[0] = vInP;
        pinLoc[1] = vInN;
        pinLoc[2] = vOut;

        // the DC voltage gain
        amp = 1;
        vmax = 0.02;
}


void OPA::stamp(MNASystem & m)
{

        // I compared this part to the QUCS source code, and it's the same!

        m.stampStatic(-1, nets[3], nets[2]);
        m.stampStatic(1, nets[2], nets[3]);

        // This is a way faster method to simulate op-amps
        m.A[nets[3]][nets[0]].gdyn.push_back(&g);
        m.A[nets[3]][nets[1]].gdyn.push_back(&ng);

        m.b[nets[3]].gdyn.push_back(&v);

        // http://qucs.sourceforge.net/tech/node67.html explains all this, really

}
void OPA::update(MNASystem & m)
{
        // This part is closer to the QUCS documentation than it is to the source code, which was hard to understand because of vague variable naming

        g = amp/(1 + pow(((2*M_PI)/2*vmax*amp*(m.b[0].lu-m.b[1].lu)), 2));

        ng = -g;

        v = g * (m.b[0].lu-m.b[1].lu) - (vmax*(2/M_PI)*atan((M_PI/(2*vmax))*amp*(m.b[0].lu-m.b[1].lu)));

}

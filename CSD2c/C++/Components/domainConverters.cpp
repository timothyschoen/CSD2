#include "../Component.h"
#include "domainConverters.h"


//
//                  DAC
//

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


//
//                  ADC
//

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

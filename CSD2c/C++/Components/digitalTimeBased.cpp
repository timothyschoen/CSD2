#include <math.h>
#include "../Component.h"
#include "digitalTimeBased.h"


//
//                  CHANGED
//

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


//
//                  HISTORY
//

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

//
//                  DELTA
//

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

//
//                 DIGITAL DELAY
//

digitalDelay::digitalDelay(std::vector<std::string> init, std::string d0, std::string d1, std::string d2)
{
        digiPins[0] = d0;
        digiPins[1] = d1;
        digiPins[2] = d2;

        int offset = 0;     // sometimes a space slips inbetween here, this offset is a fix (might not be necessary anymore?)

        if(init[0].empty()) {
                offset = 1;
        }

        if(init.size() > offset) bufSize = stoi(init[0]);
        else bufSize = 88200;
        if(init.size() > offset+1) t = stof(init[1]);
        else t = 10000;
        currentSample = 0;
        smoothTime = t;

        bufSize = pow(2, ceil(log(bufSize)/log(2)))-1;     //prepare for bitwise AND

        buf.assign(bufSize, 0);
}


void digitalDelay::updateInput(MNASystem & m)
{
        t = (int)m.getDigital(digiNets[1]) + (t>=bufSize)*bufSize;     // trying to avoid if statements

        //Write current value
        buf[currentSample] = m.getDigital(digiNets[0]);

}

void digitalDelay::update(MNASystem & m)
{

        currentSample++;
        currentSample = currentSample & bufSize;     // Bitwise AND, faster than modulo or if-statements

        smoothTime = ((1-a)*smoothTime + a * t);     // smoothing readhead movement

        readHead = (int)currentSample-smoothTime;
        readHead += (readHead<=0)*bufSize;

        m.setDigital(digiNets[2], buf[readHead]);


}


//
//                 ELAPSED
//


Elapsed::Elapsed(std::string d0)
{
        digiPins[0] = d0;
}


void Elapsed::update(MNASystem & m)
{
        m.setDigital(digiNets[0], (double)m.ticks);

}

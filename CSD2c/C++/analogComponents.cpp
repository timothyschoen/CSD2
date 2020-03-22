#include <math.h>
#include "AudioFile.h"
#include "Component.h"
#include "analogComponents.h"

// gMin for diodes etc..
constexpr double gMin = 1e-12;

// voltage tolerance
constexpr double vTolerance = 5e-5;

// thermal voltage for diodes/transistors
constexpr double vThermal = 0.026;



// Analog components

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
        // the state is 2*c*voltage - i/t0
        // so we subtract out the voltage, scale current
        // and then add the voltage back to get new state
        //
        // note that this also works if the old rate is infinite
        // (ie. t0=0) when going from DC analysis to transient
        // ?????????
        //double qq = 2*l*voltage;
        //stateVar = qq + (stateVar - qq)*told_per_new;
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
        m.stampStatic(+impedance, nets[2], nets[0]);
        m.stampStatic(-impedance, nets[2], nets[1]);
        m.stampStatic(-impedance, nets[2], nets[2]);

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





void initJunctionPN(JunctionPN & pn, double is, double n)
{
    pn.is = is;
    pn.nvt = n * vThermal;
    pn.rnvt = 1 / pn.nvt;
    pn.vcrit = pn.nvt * log(pn.nvt / (pn.is * sqrt(2.)));
}

// linearize junction at the specified voltage
//
// ideally we could handle series resistance here as well
// to avoid putting it on a separate node, but not sure how
// to make that work as it looks like we'd need Lambert-W then
void linearizeJunctionPN(JunctionPN & pn, double v)
{
    double e = pn.is * exp(v * pn.rnvt);
    double i = e - pn.is + gMin * v;
    double g = e * pn.rnvt + gMin;

    pn.geq = g;
    pn.ieq = v*g - i;
    pn.veq = v;
}

// returns true if junction is good enough
bool newtonJunctionPN(JunctionPN & pn, double v)
{
    double dv = v - pn.veq;
    if(fabs(dv) < vTolerance) return true;

    // check critical voltage and adjust voltage if over
    if(v > pn.vcrit)
    {
        // this formula comes from Qucs documentation
        v = pn.veq + pn.nvt*log((std::max)(pn.is, 1+dv*pn.rnvt));
    }

    linearizeJunctionPN(pn, v);

    return false;
}


//
//                  DIODE
//

    // l0 -->|-- l1 -- parameters default to approx 1N4148
    Diode::Diode(int l0, int l1, double rs, double is, double n) : rs(rs)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;

        initJunctionPN(pn, is, n);

        // FIXME: move init to some restart routine?

        // initial condition v = 0
        linearizeJunctionPN(pn, 0);
    }

    bool Diode::newton(MNASystem & m)
    {
        return newtonJunctionPN(pn, m.b[nets[2]].lu);
    }

    void Diode::stamp(MNASystem & m)
    {

        m.stampStatic(-1, nets[3], nets[0]);
        m.stampStatic(+1, nets[3], nets[1]);
        m.stampStatic(+1, nets[3], nets[2]);

        m.stampStatic(+1, nets[0], nets[3]);
        m.stampStatic(-1, nets[1], nets[3]);
        m.stampStatic(-1, nets[2], nets[3]);

        m.stampStatic(rs, nets[3], nets[3]);

        m.A[nets[2]][nets[2]].gdyn.push_back(&pn.geq);
        m.b[nets[2]].gdyn.push_back(&pn.ieq);


    }

    //
    //                  BIPOLAR JUNCTION TRANSISTOR
    //


    BJT::BJT(int b, int c, int e, bool pnp, std::vector<std::string> init) : pnp(pnp)
    {
        pinLoc[0] = b;
        pinLoc[1] = c;
        pinLoc[2] = e;

        if (init.size() > 0 && !init[0].empty())
        {
            type = std::stoi(init[0]);
        }
        else {
            type = 0;
        }

        // this attempts a 2n3904-style small-signal
        // transistor, although the values are a bit
        // arbitrarily set to "something reasonable"

        // forward and reverse beta
        if(type == 0) { // Simulates silicon
        bf = 250;
        br = 20;
        }
        else if (type == 1) { // Simulates germanium (this is not correct yet, germaium is louder now which it shouldn't be...)
          bf = 110;
          br = 70;
        }



        // forward and reverse alpha
        af = bf / (1 + bf);
        ar = br / (1 + br);

        // these are just rb+re and rb+rc
        // this is not necessarily the best way to
        // do anything, but having junction series
        // resistances helps handle degenerate cases
        rsbc = 5.8376+0.0001;
        rsbe = 5.8376+2.65711;

        //
        // the basic rule is that:
        //  af * ise = ar * isc = is
        //
        // FIXME: with non-equal ideality factors
        // we can get non-sensical results, why?
        //
        double is = 6.734e-15;
        double n = 1.24;
        initJunctionPN(pnE, is / af, n);
        initJunctionPN(pnC, is / ar, n);

        linearizeJunctionPN(pnE, 0);
        linearizeJunctionPN(pnC, 0);
    }

    bool BJT::newton(MNASystem & m)
    {
        return newtonJunctionPN(pnC, m.b[nets[3]].lu)
               & newtonJunctionPN(pnE, m.b[nets[4]].lu);
    }

    void BJT::stamp(MNASystem & m)
    {


        // diode currents to external base
        m.stampStatic(1-ar, nets[0], nets[5]);
        m.stampStatic(1-af, nets[0], nets[6]);

        // diode currents to external collector and emitter
        m.stampStatic(-1, nets[1], nets[5]);
        m.stampStatic(-1, nets[2], nets[6]);

        // series resistances
        m.stampStatic(rsbc, nets[5], nets[5]);
        m.stampStatic(rsbe, nets[6], nets[6]);

        // current - junction connections
        // for the PNP case we flip the signs of these
        // to flip the diode junctions wrt. the above
        if(pnp)
        {
            m.stampStatic(-1, nets[5], nets[3]);
            m.stampStatic(+1, nets[3], nets[5]);

            m.stampStatic(-1, nets[6], nets[4]);
            m.stampStatic(+1, nets[4], nets[6]);

        }
        else
        {
            m.stampStatic(+1, nets[5], nets[3]);
            m.stampStatic(-1, nets[3], nets[5]);

            m.stampStatic(+1, nets[6], nets[4]);
            m.stampStatic(-1, nets[4], nets[6]);
        }

        // external voltages to collector current
        m.stampStatic(-1, nets[5], nets[0]);
        m.stampStatic(+1, nets[5], nets[1]);

        // external voltages to emitter current
        m.stampStatic(-1, nets[6], nets[0]);
        m.stampStatic(+1, nets[6], nets[2]);

        // source transfer currents to external pins
        m.stampStatic(+ar, nets[2], nets[5]);
        m.stampStatic(+af, nets[1], nets[6]);

        // dynamic variables
        m.A[nets[3]][nets[3]].gdyn.push_back(&pnC.geq);
        m.b[nets[3]].gdyn.push_back(&pnC.ieq);

        m.A[nets[4]][nets[4]].gdyn.push_back(&pnE.geq);
        m.b[nets[4]].gdyn.push_back(&pnE.ieq);



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

      m.stampStatic(-1, nets[3], nets[2]);
      m.stampStatic(1, nets[2], nets[3]);

      // This is a way faster method to simulate op-amps
      m.A[nets[3]][nets[0]].gdyn.push_back(&g);
      m.A[nets[3]][nets[1]].gdyn.push_back(&ng);

      m.b[nets[3]].gdyn.push_back(&v);

        // http://qucs.sourceforge.net/tech/node67.html  !!!!!!!!!!!

    }
    void OPA::update(MNASystem & m)
    {
        g = amp/(1 + pow(((2*M_PI)/2*vmax*amp*(m.b[0].lu-m.b[1].lu)), 2));

        ng = -g;

        v = g * (m.b[0].lu-m.b[1].lu) - (vmax*(2/M_PI)*atan((M_PI/(2*vmax))*amp*(m.b[0].lu-m.b[1].lu)));

    }

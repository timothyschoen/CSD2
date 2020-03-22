#include <math.h>
#include "../Component.h"
#include "analogJunctions.h"


// gMin for diodes etc..
constexpr double gMin = 1e-12;

// voltage tolerance
constexpr double vTolerance = 5e-5;

// thermal voltage for diodes/transistors
constexpr double vThermal = 0.026;



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

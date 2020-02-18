
// Analog components

struct Resistor : Component<2>
{
    double  r;

    Resistor(double r, int l0, int l1) : r(r)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
    }

    void stamp(MNASystem & m) final
    {

        double g = 1. / r;
        m.stampStatic(+g, nets[0], nets[0]);
        m.stampStatic(-g, nets[0], nets[1]);
        m.stampStatic(-g, nets[1], nets[0]);
        m.stampStatic(+g, nets[1], nets[1]);
    }
};

struct AnalogDelay : Component<2, 1, 1>
{
    double  v;
    int t;
    float dw;
    int currentSample;
    double buf[88200];

    AnalogDelay(int time, float balance, int l0, int d0, int l1)
    {
        digiPins[0] = d0;
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        t = time;
        dw = balance;
        currentSample = 0;
        v = 0;
    }

    void stamp(MNASystem & m) final
    {

      m.stampStatic(-dw, nets[0], nets[2]);
      m.stampStatic(+1, nets[1], nets[2]);

      m.stampStatic(+dw, nets[2], nets[0]);
      m.stampStatic(-1, nets[2], nets[1]);

      m.b[nets[2]].gdyn.push_back(&v);

      m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
    }

    void update(MNASystem & m) final
    {
        currentSample = m.ticks;
        // Don't delay while loading
        if (currentSample >= 88200) currentSample = currentSample-88200;



        //digiNets[0] += currentSample;

        //Write current value
        buf[currentSample] = m.b[nets[0]].lu;

        // Read buffered value
        v = buf[(currentSample-t+88200)%88200];

    }

};
struct VariableResistor : Component<3>
{
    double  r;
    double  resvalue;
    double  negresvalue;

    VariableResistor(double r, int l0, int l1, int d0) : r(r)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        digiPins[0] = d0;

        resvalue = 1. / r;
        negresvalue = -resvalue;
    }

    void stamp(MNASystem & m) final
    {
        for (size_t r = 0; r < 2; r++) {
          for (size_t c = 0; c < 2; c++) {
            if(r==c) m.A[nets[r]][nets[c]].gdyn.push_back(&resvalue);
            else m.A[nets[r]][nets[c]].gdyn.push_back(&negresvalue);
          }
        }
    }
    void updateInput(MNASystem & m) final
    {
      resvalue = 1. / (r * m.digiValues[digiNets[0]]);
      negresvalue = -resvalue;
    }

};

struct Potentiometer : Component<3, 0, 1>
{
    double  r;

    double resvalue;
    double invresvalue;



    Potentiometer(double r, int l0, int l1, int l2, int d0) : r(r)
    {
        pinLoc[0] = l0; // in
        pinLoc[1] = l1; // out
        pinLoc[2] = l2; // inv out
        digiPins[0] = d0; // set value

        resvalue = 1. / r;
        invresvalue = 1. / r;
    }

    void stamp(MNASystem & m) final
    {

      /*

        for (size_t r = 0; r < 2; r++) {
          for (size_t c = 0; c < 2; c++) {
            if (r==c) {
              m.A[nets[r]][nets[c]].gdyn.push_back(&resvalue);
              m.A[nets[r * 2]][nets[c * 2]].gdyn.push_back(&invresvalue);
            }
            else  {
              m.A[nets[r]][nets[c]].gdyn.push_back(&negresvalue);
              m.A[nets[r * 2]][nets[c * 2]].gdyn.push_back(&neginvresvalue);
            }
          }
        } */

        m.stampStatic(+resvalue, nets[0], nets[0]);
        m.stampStatic(-resvalue, nets[0], nets[1]);
        m.stampStatic(-resvalue, nets[1], nets[0]);
        m.stampStatic(+resvalue, nets[1], nets[1]);

        m.stampStatic(+invresvalue, nets[0], nets[0]);
        m.stampStatic(-invresvalue, nets[0], nets[2]);
        m.stampStatic(-invresvalue, nets[2], nets[0]);
        m.stampStatic(+invresvalue, nets[2], nets[2]);

    }
    void updateInput(MNASystem & m) final
    {
      resvalue = 1. / (r * m.digiValues[digiNets[0]]);
      invresvalue = 1. / (r - (r * m.digiValues[digiNets[0]]));
    }

    void update(MNASystem & m) final
    {




    }
};

struct Capacitor : Component<2, 1>
{
    double c;
    double stateVar;
    double voltage;

    Capacitor(double c, int l0, int l1) : c(c)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;

        stateVar = 0;
        voltage = 0;
    }

    void stamp(MNASystem & m) final
    {

        // we can use a trick here, to get the capacitor to
        // work on it's own line with direct trapezoidal:
        //
        // | -g*t  +g*t  +t | v+
        // | +g*t  -g*t  -t | v-
        // | +2*g  -2*g  -1 | state
        //
        // the logic with this is that for constant timestep:
        //
        //  i1 = g*v1 - s0   , s0 = g*v0 + i0
        //  s1 = 2*g*v1 - s0 <-> s0 = 2*g*v1 - s1
        //
        // then if we substitute back:
        //  i1 = g*v1 - (2*g*v1 - s1)
        //     = s1 - g*v1
        //
        // this way we just need to copy the new state to the
        // next timestep and there's no actual integration needed
        //
        // the "half time-step" error here means that our state
        // is 2*c*v - i/t but we fix this for display in update
        // and correct the current-part on time-step changes

        // trapezoidal needs another factor of two for the g
        // since c*(v1 - v0) = (i1 + i0)/(2*t), where t = 1/T
        double g = 2*c;

        m.stampTimed(+1, nets[0], nets[2]);
        m.stampTimed(-1, nets[1], nets[2]);

        m.stampTimed(-g, nets[0], nets[0]);
        m.stampTimed(+g, nets[0], nets[1]);
        m.stampTimed(+g, nets[1], nets[0]);
        m.stampTimed(-g, nets[1], nets[1]);

        m.stampStatic(+2*g, nets[2], nets[0]);
        m.stampStatic(-2*g, nets[2], nets[1]);

        m.stampStatic(-1, nets[2], nets[2]);

        // see the comment about v:C[%d] below
        m.b[nets[2]].gdyn.push_back(&stateVar);

        // this isn't quite right as state stores 2*c*v - i/t
        // however, we'll fix this in updateFull() for display
        m.nodes[nets[2]].scale = 1 / c;
    }

    void update(MNASystem & m) final
    {
        stateVar = m.b[nets[2]].lu;

        // solve legit voltage from the pins
        voltage = m.b[nets[0]].lu - m.b[nets[1]].lu;

        // then we can store this for display here
        // since this value won't be used at this point
        m.b[nets[2]].lu = c*voltage;
    }

    void scaleTime(double told_per_new) final
    {
        // the state is 2*c*voltage - i/t0
        // so we subtract out the voltage, scale current       (!!!!!!!!!!!!!!!!!!!!!)
        // and then add the voltage back to get new state
        //
        // note that this also works if the old rate is infinite
        // (ie. t0=0) when going from DC analysis to transient
        //
        double qq = 2*c*voltage;
        stateVar = qq + (stateVar - qq)*told_per_new;
    }
};

struct Voltage : Component<2, 1>
{
    double v;

    Voltage(double v, int l0, int l1) : v(v)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
    }

    void stamp(MNASystem & m) final
    {
        m.stampStatic(-1, nets[0], nets[2]);
        m.stampStatic(+1, nets[1], nets[2]);

        m.stampStatic(+1, nets[2], nets[0]);
        m.stampStatic(-1, nets[2], nets[1]);

        m.b[nets[2]].g = v;

        m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
    }
};

// probe a differential voltage
// also forces this voltage to actually get solved :)
struct Probe : Component<2, 1>
{
    float impedance = 1;
    double average;
    double x = 0;
    double y = 0;
    double xm1 = 0;
    double ym1 = 0;
    Probe(int l0, int l1)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
    }

    void stamp(MNASystem & m) final
    {
        // vp + vn - vd = 0
        m.stampStatic(+impedance, nets[2], nets[0]);
        m.stampStatic(-impedance, nets[2], nets[1]);
        m.stampStatic(-impedance, nets[2], nets[2]);

    }
    //current = voltage/impedance
    double getAudioOutput(MNASystem & m, int channel) {

      // dc offset removal!!!
        x = m.b[nets[2]].lu * m.nodes[nets[2]].scale;
        y = x - xm1 + 0.995 * ym1;
        xm1 = x;
        ym1 = y;
      //std::cout << m.A[0][2].lu;   // -> is altijd 0!!!! zoek uit wat dit is!!
      //return m.A[0][1].lu;
      return m.b[nets[2]].lu * m.nodes[nets[2]].scale; //? Betrek current hierin!!!
      // m.A[2].lu = conductance in Siemens
      // m.A[2].lu (siemens) * m.b[2].lu (voltage) = Current
      //
    }
};


struct Printer : Component<2>
{
    float g = 2;
    Printer(int l0, int l1)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
    }

    void stamp(MNASystem & m) final
    {

        m.stampStatic(+g, nets[0], nets[0]);
        m.stampStatic(-g, nets[0], nets[1]);
        m.stampStatic(-g, nets[1], nets[0]);
        m.stampStatic(+g, nets[1], nets[1]);

    }
    //current = voltage/resistance
    void update(MNASystem & m) {
      if(m.ticks % 4410 == 0) {
        std::cout << "Voltage: " << m.b[nets[0]].lu << "V" << std::endl;
        std::cout << "Ampere: " << m.A[nets[0]][nets[1]].lu << "A" << std::endl;
        //prelu waardes zijn altijd 1... m.A.lu is altijd 0, 1 of -1 (dit is vergelijkbaar met een bepaald mna systeem!!)
        // Overweeg dat de LU solution een vector zou moeten zijn (met voltage en current)
        // Waarom is lu hier een int en geen vector? Systems zijn wel matrices en vectors, maar zijn onopgelost...
      }
    }
};

// function voltage generator
struct Function : Component<2,1>
{
    typedef double (*FuncPtr)(double t);

    FuncPtr fn;
    double  v;

    Function(FuncPtr fn, int l0, int l1) : fn(fn)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;

        v = fn(0);
    }

    void stamp(MNASystem & m) final
    {
        // this is identical to voltage source
        // except voltage is dynanic
        m.stampStatic(-1, nets[0], nets[2]);
        m.stampStatic(+1, nets[1], nets[2]);

        m.stampStatic(+1, nets[2], nets[0]);
        m.stampStatic(-1, nets[2], nets[1]);

        m.b[nets[2]].gdyn.push_back(&v);

        m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
    }

    void update(MNASystem & m) final
    {
        v = fn(m.time);
    }

};

struct InputSignal : Component<2,1>
{

    double  v;
    float freq;
    double phase;

    InputSignal(float Hz, int l0, int l1)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        freq = Hz;

        v = sin(0);
    }

    void stamp(MNASystem & m) final
    {
        // this is identical to voltage source
        // except voltage is dynanic
        m.stampStatic(-1, nets[0], nets[2]);
        m.stampStatic(+1, nets[1], nets[2]);

        m.stampStatic(+1, nets[2], nets[0]);
        m.stampStatic(-1, nets[2], nets[1]);

        m.b[nets[2]].gdyn.push_back(&v);

        m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
    }

    void update(MNASystem & m) final
    {
        phase += freq / 44100.;
        if(phase >= 1) phase = phase - 1;
        v = (sin(phase * 2. * 3.14159265358979323846)+1.)/2.;



    }

};

struct InputSample : Component<2,1>
{

    double v;
    double amplitude;
    int numSamples;
    AudioFile<double> audioFile;

    InputSample(std::string path, double inamp, int l0, int l1)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;
        amplitude = inamp;
        audioFile.load(path);
        numSamples = audioFile.getNumSamplesPerChannel();
        v = 0;
    }

    void stamp(MNASystem & m) final
    {
        // this is identical to voltage source
        // except voltage is dynanic
        m.stampStatic(-0.1, nets[0], nets[2]);
        m.stampStatic(+0.1, nets[1], nets[2]);

        m.stampStatic(+0.1, nets[2], nets[0]);
        m.stampStatic(-0.1, nets[2], nets[1]);

        m.b[nets[2]].gdyn.push_back(&v);

        m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
    }

    void update(MNASystem & m) final
    {
        if(m.ticks > 0)
          v = audioFile.samples[0][fmod(m.ticks, numSamples)]*amplitude;



    }

};


// POD-struct for PN-junction data, for diodes and BJTs
//
struct JunctionPN
{
    // variables
    double geq, ieq, veq;

    // parameters
    double is, nvt, rnvt, vcrit;
};

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

struct Diode : Component<2, 2>
{
    JunctionPN  pn;

    // should make these parameters
    double rs;

    // l0 -->|-- l1 -- parameters default to approx 1N4148
    Diode(int l0, int l1,
        double rs = 10., double is = 35e-12, double n = 1.24)
        : rs(rs)
    {
        pinLoc[0] = l0;
        pinLoc[1] = l1;

        initJunctionPN(pn, is, n);

        // FIXME: move init to some restart routine?

        // initial condition v = 0
        linearizeJunctionPN(pn, 0);
    }

    bool newton(MNASystem & m) final
    {
        return newtonJunctionPN(pn, m.b[nets[2]].lu);
    }

    void stamp(MNASystem & m) final
    {
        // Diode could be built with 3 extra nodes:
        //
        // |  .  .    .       . +1 | V+
        // |  .  .    .       . -1 | V-
        // |  .  .  grs    -grs -1 | v:D
        // |  .  . -grs grs+geq  . | v:pn = ieq
        // | -1 +1   +1       .  . | i:pn
        //
        // Here grs is the 1/rs series conductance.
        //
        // This gives us the junction voltage (v:pn) and
        // current (i:pn) and the composite voltage (v:D).
        //
        // The i:pn row is an ideal transformer connecting
        // the floating diode to the ground-referenced v:D
        // where we connect the series resistance to v:pn
        // that solves the diode equation with Newton.
        //
        // We can then add the 3rd row to the bottom 2 with
        // multipliers 1 and -rs = -1/grs and drop it:
        //
        // |  .  .   . +1 | V+
        // |  .  .   . -1 | V-
        // |  .  . geq -1 | v:pn = ieq
        // | -1 +1  +1 rs | i:pn
        //
        // Note that only the v:pn row here is non-linear.
        //
        // We could even do away without the separate row for
        // the current, which would lead to the following:
        //
        // | +grs -grs     -grs |
        // | -grs +grs     +grs |
        // | -grs +grs +grs+geq | = ieq
        //
        // In practice we keep the current row since it's
        // nice to have it as an output anyway.
        //
        m.stampStatic(-1, nets[3], nets[0]);
        m.stampStatic(+1, nets[3], nets[1]);
        m.stampStatic(+1, nets[3], nets[2]);

        m.stampStatic(+1, nets[0], nets[3]);
        m.stampStatic(-1, nets[1], nets[3]);
        m.stampStatic(-1, nets[2], nets[3]);

        m.stampStatic(rs, nets[3], nets[3]);

        m.A[nets[2]][nets[2]].gdyn.push_back(&pn.geq);
        m.b[nets[2]].gdyn.push_back(&pn.ieq);

        m.nodes[nets[3]].type = MNANodeInfo::tCurrent;

    }
};

struct BJT : Component<3, 4>
{
    // emitter and collector junctions
    JunctionPN  pnC, pnE;

    // forward and reverse alpha
    double af, ar, rsbc, rsbe;

    bool pnp;

    BJT(int b, int c, int e, bool pnp = false) : pnp(pnp)
    {
        pinLoc[0] = b;
        pinLoc[1] = c;
        pinLoc[2] = e;

        // this attempts a 2n3904-style small-signal
        // transistor, although the values are a bit
        // arbitrarily set to "something reasonable"

        // forward and reverse beta
        double bf = 200;
        double br = 20;

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

    bool newton(MNASystem & m) final
    {
        return newtonJunctionPN(pnC, m.b[nets[3]].lu)
             & newtonJunctionPN(pnE, m.b[nets[4]].lu);
    }

    void stamp(MNASystem & m) final
    {
        // The basic idea here is the same as with diodes
        // except we do it once for each junction.
        //
        // With the transfer currents sourced from the
        // diode currents, NPN then looks like this:
        //
        // 0 |  .  .  .  .  . 1-ar 1-af | vB
        // 1 |  .  .  .  .  .   -1  +af | vC
        // 2 |  .  .  .  .  .  +ar   -1 | vE
        // 3 |  .  .  . gc  .   -1    . | v:Qbc  = ic
        // 4 |  .  .  .  . ge    .   -1 | v:Qbe  = ie
        // 5 | -1 +1  . +1  . rsbc    . | i:Qbc
        // 6 | -1  . +1  . +1    . rsbe | i:Qbe
        //     ------------------------
        //      0  1  2  3  4    5    6
        //
        // For PNP version, we simply flip the junctions
        // by changing signs of (3,5),(5,3) and (4,6),(6,4).
        //
        // Also just like diodes, we have junction series
        // resistances, rather than terminal resistances.
        //
        // This works just as well, but should be kept
        // in mind when fitting particular transistors.
        //

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



        m.nodes[nets[5]].type = MNANodeInfo::tCurrent;
        m.nodes[nets[5]].scale = 1 - ar;


        m.nodes[nets[6]].type = MNANodeInfo::tCurrent;
        m.nodes[nets[6]].scale = 1 - af;
    }
};

struct OPA : Component<5, 6>
{
    // diode clamps
    JunctionPN  pnPP, pnNN;

    double g;

    // pins: out, in+, in-, supply+, supply-
    OPA(int vOut, int vInP, int vInN, int vPP, int vNN)
    {
        pinLoc[0] = vOut;
        pinLoc[1] = vInP;
        pinLoc[2] = vInN;
        pinLoc[3] = vPP;
        pinLoc[4] = vNN;

        // the DC voltage gain
        g = 10e3;

        // any sort of reasonable diode will do
        double is = 6.734e-15;
        double n = 1.24;
        initJunctionPN(pnPP, is, n);
        initJunctionPN(pnNN, is, n);

        linearizeJunctionPN(pnPP, 0);
        linearizeJunctionPN(pnNN, 0);
    }

    bool newton(MNASystem & m)
    {
        return newtonJunctionPN(pnPP, m.b[nets[5]].lu)
            & newtonJunctionPN(pnNN, m.b[nets[6]].lu);
    }

    void stamp(MNASystem & m)
    {
        // What we want here is a high-gain VCVS where
        // we then bypass to rails if we get too close.
        //
        // Here it's important not to have series resistance
        // for thee diodes, otherwise we can still exceed rails.
        //
        // NOTE: the following ignores supply currents
        //
        //      0   1   2   3  4   5   6   7   8    9
        //    vout in+ in- V+ V- vd+ vd- id+ id- iout
        // 0 |  .   .   .   .  .   .   .  +1  -1   +1 | vout
        // 1 |  .   .   .   .  .   .   .   .   .    . | in+
        // 2 |  .   .   .   .  .   .   .   .   .    . | in-
        // 3 |  .   .   .   .  .   .   .   .   .    . | v+
        // 4 |  .   .   .   .  .   .   .   .   .    . | v-
        // 5 |  .   .   .   .  . gpp   .  -1   .    . | vd+  = i0pp
        // 6 |  .   .   .   .  .   . gnn   .  -1    . | vd-  = i0nn
        // 7 | -1   .   .  +1  .  +1   .  ~0   .    . | id+  = +1.5
        // 8 | +1   .   .   . -1   .  +1   .  ~0    . | id-  = +1.5
        // 9 | -1  +g  -g   .  .   .   .   .   .   ro | iout
        //
        // We then add one useless extra row just to add
        // the currents together, so one can plot the real
        // current that actually get pushed into vOut

        // output currents
        m.stampStatic(+1, nets[0], nets[7]);
        m.stampStatic(-1, nets[0], nets[8]);
        m.stampStatic(+1, nets[0], nets[9]);

        // output feedback
        m.stampStatic(-1, nets[7], nets[0]);
        m.stampStatic(+1, nets[8], nets[0]);
        m.stampStatic(-1, nets[9], nets[0]);

        // voltage input
        m.stampStatic(+g, nets[9], nets[1]);
        m.stampStatic(-g, nets[9], nets[2]);

        // supply voltages
        m.stampStatic(+1, nets[7], nets[3]);
        m.stampStatic(-1, nets[8], nets[4]);

        // voltage drops from the supply, should be slightly
        // more than the drop voltage drop across the diodes
        m.b[nets[7]].g += 1.5;
        m.b[nets[8]].g += 1.5;

        // diode voltages to currents
        m.stampStatic(+1, nets[7], nets[5]);
        m.stampStatic(-1, nets[5], nets[7]);

        m.stampStatic(+1, nets[8], nets[6]);
        m.stampStatic(-1, nets[6], nets[8]);

        double ro = 10;

        // the series resistance for the diode clamps
        // needs to be small to handle the high gain,
        // but still use something just slightly non-zero
        double rs = gMin;

        // series resistances for diodes
        // allow "gmin" just for pivoting?
        m.stampStatic(rs, nets[7], nets[7]);
        m.stampStatic(rs, nets[8], nets[8]);

        // series output resistance
        m.stampStatic(ro, nets[9], nets[9]);

        // Finally (not show above) put a resistor between
        // the input pins.. this could be more as well.
        double ri = 50e6;

        // TODO: RECONSTRUEER DEZE CODE
        m.stampConductor(1. / ri, nets[1], nets[2]);

        // junctions
        m.A[nets[5]][nets[5]].gdyn.push_back(&pnPP.geq);
        m.A[nets[6]][nets[6]].gdyn.push_back(&pnNN.geq);

        m.b[nets[5]].gdyn.push_back(&pnPP.ieq);
        m.b[nets[6]].gdyn.push_back(&pnNN.ieq);


        m.nodes[nets[7]].type = MNANodeInfo::tCurrent;


        m.nodes[nets[8]].type = MNANodeInfo::tCurrent;


        m.nodes[nets[9]].type = MNANodeInfo::tCurrent;

        // this is useless as far as simulation goes
        // it's just for getting a nice current value
        m.stampStatic(+1, nets[10], nets[7]);
        m.stampStatic(-1, nets[10], nets[8]);
        m.stampStatic(+1, nets[10], nets[9]);
        m.stampStatic(+1, nets[10], nets[10]);


        m.nodes[nets[10]].type = MNANodeInfo::tCurrent;
    }
};

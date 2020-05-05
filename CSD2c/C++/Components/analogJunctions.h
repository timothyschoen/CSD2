

// POD-struct for PN-junction data, for diodes and BJTs
//
struct JunctionPN
{
        // variables
        double geq, ieq, veq;

        // parameters
        double is, nvt, rnvt, vcrit;
};

void initJunctionPN(JunctionPN & pn, double is, double n);

void linearizeJunctionPN(JunctionPN & pn, double v);

bool newtonJunctionPN(JunctionPN & pn, double v);


struct Diode : Component<2, 2>
{
        JunctionPN pn;

        // should make these parameters
        double rs;

        // l0 -->|-- l1 -- parameters default to approx 1N4148
        Diode(int l0, int l1, double rs = 10., double is = 35e-12, double n = 1.24);

        bool newton(MNASystem & m);

        void stamp(MNASystem & m);
};

struct BJT : Component<3, 4>
{
        // emitter and collector junctions
        JunctionPN pnC, pnE;

        // forward and reverse alpha
        double af, ar, bf, br, rsbc, rsbe;

        int type;

        bool pnp;

        BJT(int b, int c, int e, bool pnp, std::vector<std::string> init);

        bool newton(MNASystem & m);

        void stamp(MNASystem & m);
};

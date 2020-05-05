

struct Resistor : Component<2>
{
        double r;

        Resistor(double r, int l0, int l1);

        void stamp(MNASystem & m);

};



struct Capacitor : Component<2, 1>
{
        double c;
        double stateVar;
        double voltage;

        Capacitor(double c, int l0, int l1);


        void stamp(MNASystem & m);

        void update(MNASystem & m);

        void scaleTime(double told_per_new);
};

struct Inductor : Component<2, 1>
{
        double l;
        double g;
        double stateVar;
        double voltage;

        Inductor(double l, int l0, int l1);


        void stamp(MNASystem & m);

        void update(MNASystem & m);


        void scaleTime(double told_per_new);

};

struct Voltage : Component<2, 1>
{
        double v;

        Voltage(double v, int l0, int l1);

        void stamp(MNASystem & m);

};

struct Current : Component<2>
{
        double a;

        Current(double a, int l0, int l1);

        void stamp(MNASystem & m);

};


struct Transformer : Component<4, 2>
{
        double b;

        Transformer(double ratio, int inP, int inN, int outP, int outN);

        void stamp(MNASystem & m);

};



struct OPA : Component<3, 1>
{

        double amp;
        double g;
        double ng;
        double v;
        double vmax;

        OPA(int vInP, int vInN, int vOut);


        void stamp(MNASystem & m);

        void update(MNASystem & m);



};

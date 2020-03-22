




struct digitalAnalogConverter : Component<2,1,1>
{

        double v;

        digitalAnalogConverter(std::string d0, int l0, int l1);

        void stamp(MNASystem & m);

        void updateInput(MNASystem & m);

};

struct analogDigitalConverter : Component<2, 1, 1>
{
        float impedance = 1;
        double x = 0;
        double y = 0;
        double xm1 = 0;
        double ym1 = 0;
        analogDigitalConverter(int l0, int l1, std::string d0);

        void stamp(MNASystem & m);

        void update(MNASystem & m);

};

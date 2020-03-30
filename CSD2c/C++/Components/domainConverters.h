




struct digitalAnalogConverter : Component<2,1,1>
{

        double v;

        digitalAnalogConverter(std::string d0, int l0, int l1);

        void stamp(MNASystem & m);

        void updateInput(MNASystem & m);

};

struct analogDigitalConverter : Component<2, 1, 1>
{

        analogDigitalConverter(int l0, int l1, std::string d0);

        void stamp(MNASystem & m);

        void update(MNASystem & m);

};

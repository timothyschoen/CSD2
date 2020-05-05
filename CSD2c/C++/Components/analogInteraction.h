struct VariableResistor : Component<2, 0, 1>
{
        double r;
        double g;
        double ng;

        VariableResistor(int l0, int l1, std::string d0, std::vector<std::string> init);


        void stamp(MNASystem & m);

        void updateInput(MNASystem & m);


};



struct Potentiometer : Component<3, 0, 1>
{
        double r;
        double g;
        double ig;
        double input;
        double ng;
        double ing;



        Potentiometer(double r, int l0, int l1, int l2, std::string d0);


        void stamp(MNASystem & m);

        void updateInput(MNASystem & m);


};

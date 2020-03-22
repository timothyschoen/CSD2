

struct dcBlock : Component<0, 0, 2>
{
        double x = 0;
        double y = 0;
        double xm1 = 0;
        double ym1 = 0;

        dcBlock(std::string d0, std::string d1);


        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};






struct Gate : Component<0, 0, 3>
{
        double output;

        Gate(std::string d0, std::string d1, std::string d2);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};

struct Scale : Component<0, 0, 6>
{
        double input;
        double inMin;
        double inMax;
        double outMin;
        double outMax;

        Scale(std::vector<std::string> init, std::string d0, std::string d1, std::string d2, std::string d3, std::string d4, std::string d5);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};

struct Clip : Component<0, 0, 4>
{
        double input;
        double minimum;
        double maximum;


        Clip(std::vector<std::string> init, std::string d0, std::string d1, std::string d2, std::string d3);




        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};





struct mToF : Component<0, 0, 2>
{

        double input;

        mToF(std::string d0, std::string d1);


        void update(MNASystem & m);

        void updateInput(MNASystem & m);

};

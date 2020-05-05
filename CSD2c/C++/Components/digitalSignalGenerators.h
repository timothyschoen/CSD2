
struct digitalSignal : Component<0, 0, 1>
{
        double value;

        digitalSignal(double init, std::string d0);


        void update(MNASystem & m);

};

struct sineGenerator : Component<0, 0, 2>
{
        double freq;
        double phase;

        sineGenerator(std::vector<std::string> a_args, std::string d0, std::string d1);

        void updateInput(MNASystem & m);

        void update(MNASystem & m);
};

struct squareGenerator : Component<0, 0, 2>
{
        double freq;
        double phase;
        double sample = 0;

        squareGenerator(std::vector<std::string> a_args, std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);
};

struct triangleGenerator : Component<0, 0, 2>
{
        double freq;
        double phase;
        double sample = 0;

        triangleGenerator(std::vector<std::string> a_args, std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);
};

struct sawGenerator : Component<0, 0, 2>
{
        double freq;
        double phase;
        double sample = 0;

        sawGenerator(std::vector<std::string> a_args, std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);
};

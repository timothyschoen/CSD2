
struct Changed : Component<0, 0, 2>
{
        double old;
        double in;

        Changed(std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};

struct History : Component<0, 0, 2>
{
        double old;
        double in;

        History(std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};

struct getDelta : Component<0, 0, 2>
{
        double old;
        double in;

        getDelta(std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};

struct digitalDelay : Component<0, 0, 3>
{
        int t;
        int bufSize;
        int currentSample;
        double smoothTime;
        int readHead;
        double prevTime;
        std::vector<double> buf;
        float a;

        digitalDelay(std::vector<std::string> init, std::string d0, std::string d1, std::string d2);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);
};



struct Elapsed : Component<0, 0, 1>
{


        Elapsed(std::string d0);



        void update(MNASystem & m);

};

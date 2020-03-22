

struct digitalOutput : Component<0, 0, 2>
{
        double value[2];
        float amplitude;

        digitalOutput(float amp, std::string d0, std::string d1);


        void updateInput(MNASystem & m);

        double getAudioOutput(MNASystem & m, int channel);

};




struct digitalInput : Component<0, 0, 1>
{

        double amplitude;
        int numSamples;
        AudioFile<double> audioFile;

        digitalInput(std::string path, double inamp, std::string d0);



        void update(MNASystem & m);

};

struct rtDigitalInput : Component<0, 0, 1>
{

        double amplitude;
        double inputvalue;
        int tick;

        rtDigitalInput(double inamp, std::string d0, int bufsize);



        void update(MNASystem & m);


};




struct midiCtlIn : Component<0, 0, 2>
{

        int cc;
        int value;
        std::vector<unsigned char> copy;

        midiCtlIn(std::string d0, std::string d1);



        void update(MNASystem & m);

        void updateInput(MNASystem & m);

};


struct midiNoteIn : Component<0, 0, 2>
{

        int note;
        int velocity;

        std::vector<unsigned char> copy;

        midiNoteIn(std::string d0, std::string d1);



        void update(MNASystem & m);

        void updateInput(MNASystem & m);

};

struct stereoDigitalInput : Component<0,0,2>
{

        double amplitude;
        int numSamples;
        std::string inStrings[2];
        AudioFile<double> audioFile;

        stereoDigitalInput(std::string path, double inamp, std::string d0, std::string d1);



        void update(MNASystem & m);

};

struct slider : Component<0, 0, 1>
{

        float message = 0.5;
        int idx;


        slider(std::string d0, int idx);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};




struct digitalPrinter : Component<0, 0, 1>
{
        int perTicks;
        int tick;
        double input;

        digitalPrinter(std::vector<std::string> a_args, std::string d0);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);
};

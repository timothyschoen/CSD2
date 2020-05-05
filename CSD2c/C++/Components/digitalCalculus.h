#include <functional>




// Basic arithmetics
struct digitalArithmetic : Component<0, 0, 3>
{
        double outvalue;
        double value;
        std::string name;
        double input;
        std::function<void()> fun;

        digitalArithmetic(std::string type, std::vector<std::string> init, std::string d0, std::string d1, std::string d2);


        void stamp(MNASystem & m);

        void updateInput(MNASystem & m);

        void update(MNASystem & m);
};

struct logarithms : Component<0, 0, 2>
{
        double input;
        std::string name;
        std::function<void()> fun;

        logarithms(std::string type, std::string d0, std::string d1);

        void stamp(MNASystem & m);

        void update(MNASystem & m);

};

struct accumulate : Component<0, 0, 2>
{
        long double accum;

        accumulate(std::string d0, std::string d1);




        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};


struct absol : Component<0, 0, 2>
{
        double input;

        absol(std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};
struct flor : Component<0, 0, 2>
{
        double input;

        flor(std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);
};

struct ceiling : Component<0, 0, 2>
{
        double input;

        ceiling(std::string d0, std::string d1);



        void updateInput(MNASystem & m);

        void update(MNASystem & m);

};

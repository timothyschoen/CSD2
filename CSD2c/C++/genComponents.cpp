#include <initializer_list>
#include "./gen/phasor.h"
#include "./gen/count.h"

// Component2s from gendsp

namespace Patcher = phasor;

struct genLoader : Component2<0, 0, 3>
{

    CommonState* gState = NULL;
    t_sample* gInputs = NULL;
    t_sample* gOutputs = NULL;
    int nInChannels;
    int nOutChannels;
    int nDigipins;
    const int* nPins = &nDigipins;
    t_sample* ip[24]; // TODO: set dynamically, altough 24 seems like a reasonable limit
    t_sample* op[24];
    std::vector<double> inputbuff;



    genLoader(std::string nom, std::initializer_list<std::string> a_args)
    {
        // Set variable patcher to the name of the patcher we want to load
        setPatcher(nom);

        // Get number of ins and outs
        nInChannels = Patcher::num_inputs();
        nOutChannels = Patcher::num_outputs();
        nDigipins = nInChannels + nOutChannels;

        inputbuff.resize(nInChannels);

        // Change the number of inputs/outputs to what the gen patcher has
        digitalReset(nPins);

        // Get all arguments
        auto mylist = a_args.begin();
        std::cout << a_args.size() << '\n';

        // Map them to pins
        for (size_t i = 0; i < a_args.size(); i++) {
            digiPins[i] = mylist[i];
        }

        // Buffers
        gInputs = new t_sample[nInChannels];
        gOutputs = new t_sample[nOutChannels];

        // Initialize Gen patcher
        gState = (CommonState*)Patcher::create(44100, 1);

    }

    void setPatcher(std::string name) {
        if(!name.compare("phasor"))
            namespace Patcher = phasor;
        else if (!name.compare("count"))
            namespace Patcher = count;
    }

    void stamp(MNASystem & m) final
    {
    }

    void updateInput(MNASystem & m) final
    {

        // Setting in

        for (int i = 0; i < nInChannels; i++) {

            *(gInputs + i) = m.getDigital(digiNets[i]);

            inputbuff[i] = *(gInputs + i);

        }


    }

    void update(MNASystem & m) final
    {

        for (int i = 0; i < nInChannels; i++) {
            ip[i] = &inputbuff[i];
        }

        op[0] = gOutputs + 1;

        // Run the patch!!
        Patcher::perform(gState, ip, 2, op, 1, 1);
        // Getting output
        for (int i = 0; i < nOutChannels; i++) {
            m.setDigital(digiNets[nInChannels+i], *op[0]);
        }

    }

};

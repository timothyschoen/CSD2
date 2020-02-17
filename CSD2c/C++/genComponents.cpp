#include <initializer_list>
#include "./gen/phasor.h"
#include "./gen/count.h"

// Components from gendsp

namespace Patcher = phasor;

struct genLoader : Component<0, 0, 3>
{

    CommonState* gState = NULL;
    t_sample* gInputs = NULL;
    t_sample* gOutputs = NULL;
    int nInChannels;
    int nOutChannels;
    int nDigipins;
    const int* nPins = &nDigipins;


    genLoader(std::string nom, std::initializer_list<int> a_args)
    {
        // Set variable patcher to the name of the patcher we want to load
        setPatcher(nom);

        // Get number of ins and outs
        nInChannels = Patcher::num_inputs();
        nOutChannels = Patcher::num_outputs();
        nDigipins = nInChannels + nOutChannels;

        // Input buffers
        gInputs = new t_sample[nInChannels];
        gOutputs = new t_sample[nOutChannels];

        // Initialize Gen patcher
        gState = (CommonState*)Patcher::create(44100, 1);

        // Change the number of inputs/outputs to what the gen patcher has
        digitalReset(nPins);

        // Get all arguments
        auto mylist = a_args.begin();
        std::cout << a_args.size() << '\n';

        // Map them to pins
        for (size_t i = 0; i < a_args.size(); i++) {
          digiPins[i] = mylist[i];
        }

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

    void update(MNASystem & m) final
    {
    t_sample* ip[nInChannels];
    t_sample* op[nOutChannels];

    // Setting in
  	for (int i = 0; i < nInChannels; i++) {

  			*(gInputs + i) = m.digiValues[digiNets[i]];
  		ip[i] = gInputs + i;
  	}
    op[0] = gOutputs + 1;

    // Run the patch!!
    Patcher::perform(gState, ip, 2, op, 1, 1);

    // Getting output
    for (int i = 0; i < nOutChannels; i++) {
    m.digiValues[digiNets[nInChannels+i]] = *op[0];
  }

    }

};

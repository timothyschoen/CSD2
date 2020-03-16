#include "IComponent.h"

template <int nPins = 0, int nInternalNets = 0, int nDigipins = 0>
struct Component : IComponent
{
    static const int nNets = nPins + nInternalNets;

    int pinLoc[nPins];
    int nets[nNets];
    std::vector<std::vector<int>> digiNets = std::vector<std::vector<int>>(nDigipins);
    std::vector<std::string> digiPins = std::vector<std::string>(nDigipins);

    int pinCount() final
    {
        return nPins;
    }

    const int* getPinLocs() const final
    {
        return pinLoc;    // This is used for running setupnets!!!! (for linking pinloc to nets)
    }
    const std::vector<std::string> getDigiLocs() const final
    {
        return digiPins;
    }


    void setupNets(int & netSize, int & states, const int* pins, const std::vector<std::string> digipins) final
    {
        for(int i = 0; i < nPins; ++i)
        {
            nets[i] = pins[i];
        }

        for(int i = 0; i < nDigipins; ++i)
        {
            std::string connections = digipins[i];
            connections.erase(0, 1);
            connections.erase(connections.size()-1, 1);
            std::stringstream ss(connections);
            std::string obj;
            std::vector<int> inputs;
            if(connections.size() > 2)
            {

                while(std::getline(ss,obj,':'))
                {
                    inputs.push_back(std::stoi(obj));
                }
            }
            else if (connections.size() != 0)
            {
                inputs.push_back(std::stoi(connections));
            }

            digiNets[i] = inputs;
        }

        for(int i = 0; i < nInternalNets; ++i)
        {
            nets[nPins + i] = netSize++;
        }

        setupStates(states);
    }

};

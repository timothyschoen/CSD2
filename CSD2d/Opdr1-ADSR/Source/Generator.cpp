#include <unistd.h>
#include <iostream>
#include <cmath>
#include "Generator.h"

//Constructor
Generator::Generator() {
    frequency = 0;
}

//Constructor
Generator::~Generator() {
}

//Function we can overwrite for the filter, will do nothing on an oscillator subclass
void Generator::setPitch(int pitch) {
  frequency = mtof(pitch+transpose);
  // This will trigger a recalculation of the filter, it will execute an empty virtual function for other classes
  calc();
}

void Generator::setTranspose(float amount) {
    transpose = amount;
}

double Generator::getSample(float amplitude) {
    return output * amplitude;
}


//Midi to Frequency function
double Generator::mtof(int input) {
  float freq = 440 * pow(2, ((float)input - 49) / 12);
  return freq;
}

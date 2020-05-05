#include <unistd.h>
#include <iostream>
#include <cmath>
#include "SynthClass.h"

//Constructor
Synth::Synth() {
    frequency = 0;
}

//Constructor
Synth::~Synth() {
}

//Function we can overwrite for the filter, will do nothing on an oscillator subclass
void Synth::setPitch(int pitch) {
  frequency = mtof(pitch+transpose);
  // This will trigger a recalculation of the filter, it will execute an empty virtual function for oscs
  calc();
}

void Synth::setTranspose(float amount) {
    transpose = amount;
}


//Midi to Frequency function
double Synth::mtof(int input) {
  float output = 440 * pow(2, ((float)input - 49) / 12);
  return output;
}

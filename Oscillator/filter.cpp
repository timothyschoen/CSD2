#include <unistd.h>
#include <iostream>
#include <cmath>

class Filter {
  double samplerate = 44100;
  float cutoff = 20000;
  float resonance = 0.2;

  public:
  Filter(double samplerate) {
    this->samplerate = samplerate;
 }
 void tick() {

 }
 double getSample() {
   
 }


};

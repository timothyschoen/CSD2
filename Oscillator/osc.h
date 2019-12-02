#include <unistd.h>
#include <iostream>
#include <cmath>


class Oscillator {
  double samplerate = 44100;
  double phase = 0;
  double sample = 0;
  double output = 0;
  public:
   Oscillator(int samplerate) {
  }
  double sine() {
    sample = sin(phase * 2 * 3.14159265358979323846);
    return sample;
  }
  double sawtooth() {
    sample = (1-phase)-0.5;
    return sample;
  }
  double square() {
    if(phase >= 0.5) sample = 0.7; // 1 and -1 makes it significantly louder than the sine and saw
    else sample = -0.7;
    return sample;
  }

  double (Oscillator::*shapePointers[3])() = {&Oscillator::sine, &Oscillator::sawtooth, &Oscillator::square};

  void tick(double freq) {
    phase += freq / samplerate;
    if(phase >= 1) phase = phase - 1;
  }
  double getSample(int shape, double amplitude) {
    output = (this->*shapePointers[shape])()*amplitude;
    return output;
  }
};

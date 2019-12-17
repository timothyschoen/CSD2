#include <unistd.h>
#include <iostream>
#include <cmath>
#include "oscillator.h"


// Constructor Class

   Oscillator::Oscillator(unsigned long samplerate) {
     this->samplerate = samplerate;
     phase = 0;
     sample = 0;
     output = 0;
     twoPI = 2 * 3.14159265358979323846;
   }

   Oscillator::~Oscillator()
   {
   }
  // Calculation for sine
  double Oscillator::sine() {
    sample = sin(phase * 2 * 3.14159265358979323846);
    return sample;
  }
  // Calculation for saw
  double Oscillator::sawtooth() {
    sample = (1-phase)-0.5;
    return sample;
  }
  double Oscillator::square() {
    if(phase >= 0.5) sample = 0.7; // 1 and -1 makes it significantly louder than the sine and saw
    else sample = -0.7;
    return sample;
  }
  double Oscillator::triangle() {
    sample = -1.0 + (2.0 * phase);
    sample = 0.9 * (fabs(sample) - 0.5);
    return sample;
  }
  // Poor attempt at PolyBLEP anti-aliasing
  double Oscillator::poly_blep(double t) {
    double dt = frequency / samplerate;
    // 0 <= t < 1
    if (t < dt) {
        t /= dt;
        return t+t - t*t - 1.0;
    }
    // -1 < t < 0
    else if (t > 1.0 - dt) {
        t = (t - 1.0) / dt;
        return t*t + t+t + 1.0;
    }
    // 0 otherwise
    else return 0.0;
}

  // Array with pointers to sine, saw and square functions

  // Move phase value to the next sample
  void Oscillator::tick() {
    phase += frequency / samplerate;
    t = phase / twoPI;
    if(phase >= 1) phase = phase - 1;
  }

  // Retrieve sample value
  double Oscillator::getSample(int shape, double amplitude) {
    output = (this->*shapePointers[shape])()*amplitude;
    if (output > 0.9) {
    output = 0.9;
    }
    else if(output < -0.9) {
    output = -0.9;
    }
    return output;
  }

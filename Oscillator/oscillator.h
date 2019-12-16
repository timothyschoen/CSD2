#include <unistd.h>
#include <iostream>
#include <cmath>
#include "SynthClass.h"
#pragma once


class Oscillator : public Synth {

// Constructor Class
public:
   Oscillator(unsigned long samplerate);
    ~Oscillator();
   // Move phase value to the next sample
   void tick();
   // Retrieve sample value
   double getSample(int shape, double amplitude);
   void lfoMode(bool low);
   double sine();
   double sawtooth();
   double square();
   double triangle();
   double (Oscillator::*shapePointers[4])() = {&Oscillator::sine, &Oscillator::sawtooth, &Oscillator::square, &Oscillator::triangle};

private:
  double samplerate;
  double phase;
  double sample;
  double output;
  double t;
  double freq;
  double twoPI = 2 * 3.14159265358979323846;
  // Calculation for sine

  double poly_blep(double t);

  // Array with pointers to sine, saw and square functions

  //double (Oscillator::*shapePointers[4])();
};

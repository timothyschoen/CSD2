#include <unistd.h>
#include <iostream>
#include <cmath>
#include "Generator.h"
#pragma once


class Oscillator : public Generator {

// Constructor Class
public:
   Oscillator(unsigned long samplerate);
    ~Oscillator();
   // Move phase value to the next sample
   void tick();
   // Retrieve sample value
   void setShape(int shp);



private:
  int shape;
  double samplerate;
  double phase;
  double sample;
  double t;
  double twoPI = 2 * 3.14159265358979323846;
    
  double sine();
  double sawtooth();
  double square();
  double triangle();

//Pointers to the waveshape generators
double (Oscillator::*shapePointers[4])() = {&Oscillator::sine, &Oscillator::sawtooth, &Oscillator::square, &Oscillator::triangle};

};

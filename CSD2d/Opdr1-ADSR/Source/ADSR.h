#include <unistd.h>
#include <iostream>
#include <cmath>
#include "Generator.h"
#pragma once


class Envelope : public Generator {
  int state = 0;
  bool playing = false;
  float target[3] = {1, 0.3, 0};
  float increments[3] = {0.01, -0.2, -0.01};
  bool released;


public:
  Envelope(float attack, float decay, float sustain, float release);
  ~Envelope();

  void tick();

  void noteOff();

  void noteOn();

  void setAttack(float value);

  void setDecay(float value);

  void setSustain(float value);

  void setRelease(float value);

  //Checks if voice is free
  bool getReleased();
};

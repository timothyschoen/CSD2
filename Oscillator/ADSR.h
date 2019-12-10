#include <unistd.h>
#include <iostream>
#include <cmath>
#include "SynthClass.h"
#pragma once


class ADSR : public Synth {
  int state = 0;
  double level = 0.01;
  bool playing = false;
  float target[3] = {1, 0.3, 0};
  float increments[3] = {0.01, -0.2, -0.01};

public:
  ADSR(float attack, float decay, float sustain, float release);
  ~ADSR();

  void tick();

  double getValue();

  void noteOff();

  void noteOn();

  void setAttack(float value);

  void setDecay(float value);

  void setSustain(float value);

  void setRelease(float value);

};

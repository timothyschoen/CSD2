#include <unistd.h>
#include <iostream>
#include <cmath>
#include "ADSR.h"

ADSR::ADSR(float attack, float decay, float sustain, float release) {
  increments[0] = 1/attack;
  increments[1] = (1/decay)*-1;
  increments[2] = (1/release)*-1;
  target[1] = sustain;
}

ADSR::~ADSR()
{
}


  void ADSR::tick() {
    if (playing == true) {
    level += increments[state];
    }
    if((level >= target[state] && state == 0) || (level <= target[state] && state == 2)) {
    state++;
    }
    else if (state == 1 && level <= target[state]) {
    playing = false;
    state++;
    }
    if (state > 2) {
    level = 0;
    playing = false;
    }
  }
  double ADSR::getValue() {
  return level;
  }
  void ADSR::noteOff() {
  playing = true;
  state = 2;
  }
  void ADSR::noteOn() {
  playing = true;
  state = 0;
  }
  void ADSR::setAttack(float value){
  increments[0] = 1/value;
  }
  void ADSR::setDecay(float value){
  increments[1] = (float)(1/value)*-1;
  }
  void ADSR::setSustain(float value){
  target[1] = value;
  }
  void ADSR::setRelease(float value){
  increments[2] = (float)(1/value)*-1;
  }

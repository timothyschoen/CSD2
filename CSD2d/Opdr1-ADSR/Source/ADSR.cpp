#include <unistd.h>
#include <iostream>
#include <cmath>
#include "ADSR.h"

  //Constructor class
  Envelope::Envelope(float attack, float decay, float sustain, float release) {
    increments[0] = 1/(attack+0.001); // 1/attack leads to the increment we need when we poll every 1ms
    increments[1] = (1/(decay+0.001))*-1; // Add a small number to make sure we never divide by 0!!!
    increments[2] = (1/(release+0.001))*-1;
    target[1] = sustain;
    released = true;
  }

  Envelope::~Envelope()
  {
  }

  // Move the Envelopee to the next value
  void Envelope::tick() {
    //Never go above 1!
    if (level > 1) {
    level = 1;
    }
    // Add the increment if we are playing (attack, decay and release parts)
    if (playing == true) {
    level += increments[state];
    }
    // If we have arrived at our attack or release target, move to the next phase
    if((level >= target[state] && state == 0) || (level <= target[state] && state == 2)) {
    state++;
    }
    // If we reach our sustain phase, stop moving and hold until note-off
    else if (state == 1 && level <= target[state]) {
    playing = false;
    state++;
    }
    // When release is done, stop playing and send a release signal to voice manager
    if (state > 2) {
    level = 0;
    playing = false;
    released = true;
    }

  // Return current value
  }
  double Envelope::getValue() {
  return level;
  }
  // Move to release phase immediately
  void Envelope::noteOff() {
  playing = true;
  state = 2;
  }
  //Note-on starts the Envelopee at the beginning again
  void Envelope::noteOn() {
  playing = true;
  released = false;
  state = 0;
  }
  // Set out Envelope values
  //1/x leads to the correct increment we need when we poll every 1ms
  // Add a small number to make sure we never divide by 0!!! (infinite volume hurts your ears!)
  void Envelope::setAttack(float value){
  increments[0] = 1/(value+0.001);
  }
  void Envelope::setDecay(float value){
  increments[1] = (float)(1/(value+0.001))*-1;
  }
  void Envelope::setSustain(float value){
  target[1] = value;
  }
  void Envelope::setRelease(float value){
  increments[2] = (float)(1/(value+0.001))*-1;
  }
  // Check if the Envelopee is done, to see if the voice can be freed
  bool Envelope::getReleased() {
    return released;
  }

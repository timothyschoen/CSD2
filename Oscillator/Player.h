#include <unistd.h>
#include <iostream>
#include <thread>
#pragma once
#include <chrono>
#include <vector>
#include "SynthClass.h"
#include "./rtmidi-master/RtMidi.h"

class Player : public Synth {
  public:
    Player();
    int getNote();
    void newChord();

  private:
      int sleeptime;
      int newnote;
      bool playing;
      int rootnote = 1;
      int beat;
      int majorscale[16] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23};
      int rhythmlist[8] = {1, 2, 2, 2, 3, 4, 4, 4};
      int chord[4];
      int oldnote;
      int direction;
      int scaleindex;
      typedef std::chrono::high_resolution_clock clock;
      typedef std::chrono::duration<float, std::milli> duration;
      clock::time_point start;



};

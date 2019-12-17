#include <unistd.h>
#include <iostream>
#include <chrono>
#include "Player.h"
#include "./rtmidi-master/RtMidi.h"


  //Constructor
  //Major Scale defined in header!
  Player::Player() {
    newnote = 0;
    oldnote = 0;
    playing = false;
    beat = 0;
    direction = 1;
    scaleindex = 0;
    sleeptime = 200;
    //Set start time
    start = clock::now();
  }
  void Player::newChord() {
    rootnote = rand()%7;
    for(int i = 0; i < 4; i++) {
      chord[i] = majorscale[rootnote+(i*2)];
    }

  }
  // Make a new note and return it
  int Player::getNote() {
    // Get the elapsed time since start or the last note, in ms
    duration elapsed = clock::now() - start;
    //Note-offs before the next note starts playing
    if (elapsed.count() >= sleeptime*0.8 && playing) {
      playing = false;
      // 128+note means note-off because returning arrays is hard?
      return 128+newnote;
    }
    // Every 'sleeptime' ms, make a note-on
    if (elapsed.count() >= sleeptime && !playing) {
      sleeptime = rhythmlist[rand()%8]*100;
      // I don't think this still has a function??
      beat = (beat + 1)%16;
      // Randomize direction while remaining in bounds
      if (scaleindex <= 0) {
        direction = (rand()%2);
      }
      else if (scaleindex >= 16) {
        direction = (rand()%2)-1;
      }
      else {
        direction = (rand()%3)-1;
      }
      // Get the new scale index
      scaleindex = (scaleindex+direction)%16;
      // Save previous note
      oldnote = newnote;
      // Get a new note from the scale
      if (beat%2 == 0 && rand()%4 != 0) {
        newnote = chord[scaleindex%4]+48;
      }
      else if (beat%2 == 1 && rand()%3 != 0) {
        newnote = majorscale[scaleindex]+48;
      }
      else {
        newnote = 0;
      }
      if (beat%8 == 0) {
        newChord();
      }
      // We had a note, so set a new start point for counting
      // If we had a rest we still want to restart counting!
      start = clock::now();
      // We are playing a note now
      playing = true;
      // Only output if the note changed
      if (oldnote != newnote) {
          return newnote;
      }

      }
    else {
      // 0 means rest
      return 0;
    }
    return 0;
  }

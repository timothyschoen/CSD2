#include <unistd.h>
#include <iostream>
#include "MidiInstrumentClass.h"

class Player {
public:
  Player() {
    int beat = 0;
    int newnote = 0;
    int direction = 1;
    int scaleindex = 0;
    int majorscale[] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23};
    while(true) {
      beat = (beat + 1)%16;
      int randVar = rand() % 4;
      direction = (rand()%3)-1;
      scaleindex = scaleindex+direction;
      newnote = majorscale[scaleindex];
      if ((beat%4 == 0 && randVar != 1) || randVar == 0) {

        inst2.play(newnote+24);
      }

      if (beat%2 == 0){
        usleep(220000);
      }
      else {
        usleep(220000);
      }
    }
    return 0;
  }
  int getNote() {
  result = newnote;
  newnote = 0;
  return result;
  }
}

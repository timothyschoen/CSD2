#include <unistd.h>
#include <iostream>
#include "InstrumentClass.h"


int main() {
  int i = 0;
  Inst::Instrument inst1("hey");
  Inst::Instrument inst2("hey");
  while(true) {
    int randVar =  rand() % 4;
    std::string arr[] = { "Boem", "Wheep", "Pew Pew", "Dszshshs"};
    inst1.play(arr[randVar]);
    randVar =  rand() % 4;
    inst2.play(arr[randVar]);
    sleep(1);
  }
  return 0;
}

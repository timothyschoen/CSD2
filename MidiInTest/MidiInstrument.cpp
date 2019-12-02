#include <unistd.h>
#include <iostream>
#include "./rtmidi-master/RtMidi.h"


int main() {
  RtMidiIn *midiin = new RtMidiIn();
  std::vector<unsigned char> message;
  int nBytes, i;
  double stamp;
  midiin->openVirtualPort("testinput");
  while (true) {
    stamp = midiin->getMessage( &message );
    nBytes = message.size();
    for ( i=0; i<nBytes; i++ )
      std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
    if ( nBytes > 0 )
      std::cout << "stamp = " << stamp << std::endl;
    // Sleep for 10 milliseconds ... platform-dependent.
    usleep( 1000 );
  }

  return 0;
}

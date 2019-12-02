#include <unistd.h>
#include "./rtmidi-master/RtMidi.h"

namespace Inst
{
class Instrument {
  RtMidiOut *midiout;
  int lastnote;
  public: Instrument(std::string name) {
    this->midiout = new RtMidiOut();
    unsigned int nPorts = midiout->getPortCount();
    std::cout << nPorts;
    if ( nPorts == 0 ) {
      std::cout << "No ports available!\n";
    }
    midiout->openVirtualPort(name);

}
  int play(int note) {
    std::vector<unsigned char> message;
    message.push_back( 192 );
    message.push_back( 5 );
    message.push_back( 5 );
    message[0] = 144;
    message[1] = note;
    message[2] = 90;
    this->lastnote = note;
    this->midiout->sendMessage( &message );
    return 0;
  }
  int stop() {
    std::vector<unsigned char> message;
    message.push_back( 192 );
    message.push_back( 5 );
    message.push_back( 5 );
    message[0] = 128;
    message[1] = this->lastnote;
    message[2] = 0;
    this->midiout->sendMessage( &message );
    return 0;
  }
};
}


// g++ -Wall -D__MACOSX_CORE__ -o MidiInstrument MidiInstrument.cpp ./rtmidi/RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation

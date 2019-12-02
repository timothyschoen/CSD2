#include <unistd.h>
#include <iostream>
#include <cmath>
#include <thread>
#include "osc.h"
#include <math.h>
#include "jack_module.h"
#include "./rtmidi-master/RtMidi.h"

int choice = 1;
int tempvar = 0;

void keyinput()
{
  while(true) {
  std::cout << "What kind of waveshape do you want??" << std::endl << "1: Sine" << std::endl << "2: Saw" << std::endl << "3: Square" << std::endl;
  std::cin >> tempvar;
  if (tempvar > 0 && tempvar <= 3 && std::cin) {
    choice = tempvar;
  }
  else {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  std::cout << std::endl << std::endl;
  }

}

int main(int argc,char **argv) {

  std::thread inthread (keyinput);
  // Initialize Jack server
  JackModule jack;
  jack.init("testserver");
  Oscillator osc1(jack.getSamplerate());

  // Default frequency
  float frequency = 400;

  // Function to execute for each sample
  jack.onProcess = [&osc1, &frequency](jack_default_audio_sample_t *inBuf,
     jack_default_audio_sample_t *outBuf, jack_nframes_t nframes)
  {
    //loop through frames, retrieve sample of sine per frame
    for(int i = 0; i < nframes; i++) {
      //TODO check type of jack_default_audio_sample_t, double? or float?
      osc1.tick(frequency);
      outBuf[i] = osc1.getSample(choice-1, 0.2);
    }
    return 0;
  };
  jack.autoConnect();


  // Empty midi message
  std::vector<unsigned char> message;
  int nBytes;
  double stamp;

// TODO: Start implementing polyphony:

  bool noteBusy[4] = {false, false, false, false};

  // Start RtMidi and open virtual port
  RtMidiIn *midiin = new RtMidiIn();
  midiin->openVirtualPort("test");
  midiin->ignoreTypes( false, false, false );

  while (true) {
    // See if there is a message
    stamp = midiin->getMessage( &message );
    nBytes = message.size();
    // If yes, update frequency
    if ( nBytes > 0 ) {
      // If it's a note-on
      if ((int)message[0] == 144) {
        // MtoF
        frequency = 440 * pow(2, ((float)message[1]- 49) / 12);
        }
      if ((int)message[0] == 128 || (int)message[2] == 0) {
        // MtoF
        frequency = 0;
        }
    }
    // Sleep for 1000 microseconds
    usleep(1000);
  }
  sleep(-1);
}

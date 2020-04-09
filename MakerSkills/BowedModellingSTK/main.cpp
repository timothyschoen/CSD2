#include <iostream>
#include <unistd.h>
#include <thread>
#include "Bowed.h"
#include "include/RtAudio.h"
#include <filesystem>
#include <lo/lo.h>
#include <lo/lo_cpp.h>

// Initialize bowedString with a lower bound of 50hz
stk::Bowed bowedString(50);

// rtAudio variables
unsigned int bufferFrames, fs = 44100, offset = 0;
unsigned int channs;
RtAudio::StreamOptions options;
const unsigned int callbackReturnValue = 1;
int buffersize = 512;
unsigned int frameCounter = 0;
bool checkCount = false;
unsigned int nFrames = 0;
unsigned int bufferBytes;




// rtAudio callback function
int audioloop( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double /*streamTime*/, RtAudioStreamStatus status, void *data )
{
        unsigned int i;
        double samp;
        extern unsigned int channs;
        double *buffer = (double *) outputBuffer; // output buffer that we pass back to rtaudio



        for ( i=0; i<nBufferFrames; i++ )
        {
                // Get the sample from bowedstring
                samp = bowedString.tick();
                *buffer++ = samp; // get left output
                *buffer++ = samp; // get right output

        }


        frameCounter += nBufferFrames;
        if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
        return 0;
}

// rtAudio setup
void realtime_audio() {
  RtAudio dac;

  if ( dac.getDeviceCount() < 1 )
  {
          std::cout << "\nNo audio devices found!\n";
          exit( 1 );
  }

  channs = 2;


  double *data = (double *) calloc( channs, sizeof( double ) );

  // Let RtAudio print messages to stderr.
  dac.showWarnings( true );

  // Set our stream parameters
  bufferFrames = buffersize;
  RtAudio::StreamParameters oParams;
  oParams.deviceId = dac.getDefaultOutputDevice();
  oParams.nChannels = channs;
  oParams.firstChannel = offset;

  RtAudio::StreamParameters iParams;
  iParams.deviceId = dac.getDefaultInputDevice();
  iParams.nChannels = 1;
  iParams.firstChannel = offset;

  options.flags = RTAUDIO_HOG_DEVICE;
  options.flags = RTAUDIO_SCHEDULE_REALTIME;

  try
  {
          // Open the stream, passes the audio processing function
          dac.openStream( &oParams, &iParams, RTAUDIO_FLOAT64, fs, &bufferFrames, &audioloop, (void *)&bufferBytes, &options );
          dac.startStream();
  }
  catch ( RtAudioError& e )
  {
          e.printMessage();
          goto cleanup;
  }

  if ( checkCount )
  {
          while ( dac.isStreamRunning() == true ) usleep( 100 );
  }
  else
  {
          std::cout << "Stream latency = " << dac.getStreamLatency() << "\n" << std::endl;



  }

	while(true) {
		//std::cout << "hey" << '\n';
		usleep(2000);
	}

cleanup:
  if ( dac.isStreamOpen() ) dac.closeStream();
  free( data );

}


float mtof(float input) {
  float output = 440. * pow(2, ((float)input - 49.) / 12.);
  return output;
}


int main()
{

  std::filesystem::path cwd = std::filesystem::current_path() / "mouse_pc.py";

  std::string pypath = std::string("python3 ").append(cwd.string());


  std::thread t1(system, pypath.c_str());

  // Add a little vibrato
	bowedString.controlChange(11, 50);
	bowedString.controlChange(1, 5);


  // Set up OSC server
	lo::ServerThread st(9002);
	st.start();




	//keep the program running and listen for user input, q = quit
	std::cout << "\n\nPress 'q' when you want to quit the program.\n";

	// set up osc listeners
	st.add_method("/n", "f",   // Start bowing
								[&](lo_arg **argv, float){
					bowedString.startBowing(argv[0]->f, 0.1);
					bowedString.controlChange(2, argv[0]->f*5);
          bowedString.controlChange(1, argv[0]->f*8);
	});
	st.add_method("/o", "f", // Stop bowing
								[&](lo_arg **argv, float){
					bowedString.stopBowing(1);
	});
	st.add_method("/v", "f",
								[&](lo_arg **argv, float){

	});
  st.add_method("/f", "f",
                [&](lo_arg **argv, float){
                bowedString.setFrequency(mtof(argv[0]->f));
  });

	realtime_audio();

  t1.join();

	//end the program
	return 0;

}

#include <iostream>
#include <cstdlib>

#include "jack_module.h"


int main()
{
  // create a JackModule instance
  JackModule jack;
  // init the jack, use program name as JACK client name
  jack.init();

  jack.onProcess = [&](jack_default_audio_sample_t *inBuf, jack_default_audio_sample_t *outBuf,
    jack_nframes_t nframes) {
      return 0;
    };




  jack.autoConnect();
	//keep the program running and listen for user input, q = quit
	std::cout << "\n\nPress 'q' when you want to quit the program.\n";
	bool running = true;
	while (running)
	{
		switch (std::cin.get())
		{
			case 'q':
				running = false;

				jack.end();
				break;
		}
	}

	//end the program
	return 0;

}

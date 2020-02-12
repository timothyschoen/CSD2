CC = g++
CFLAGS = -Wall -std=c++11
LDFLAGS= -ljack

all: Halite

Halite : jack_module.o
	$(CC) -o $@ $(CFLAGS) main.cpp jack_module.o $(LDFLAGS)
	mv Halite ../compiled/

jack_module.o:
	$(CC) -c $< $(CFLAGS) ./jack/jack_module.cpp $(LDFLAGS)

clean:
	rm -f *.o
	rm -f example

















#MidiSynth : synth.cpp
#		g++ -Wall -D__MACOSX_CORE__ -std=c++11 -ljack synth.cpp SynthClass.cpp oscillator.cpp ADSR.cpp  MoogFilter.cpp ./rtmidi-master/RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
#
#clean:
	rm -f *.o

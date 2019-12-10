CC = g++
CFLAGS = -I/usr/local/include -Wall -std=c++11
LDFLAGS= -ljack
CAFLAGS= -D__MACOSX_CORE__ -framework CoreMIDI -framework CoreAudio -framework CoreFoundation

all: MidiSynth

MidiSynth : SynthClass.o Oscillator.o jack_module.o ADSR.o MoogFilter.o RtMidi.o
	$(CC) -o $@ $(CFLAGS) SynthClass.o Oscillator.o jack_module.o ADSR.o MoogFilter.o RtMidi.o synth.cpp $(LDFLAGS) $(CAFLAGS)

SynthClass.o:
	$(CC) -c $< $(CFLAGS) SynthClass.cpp

jack_module.o:
	$(CC) -c $< $(CFLAGS) ./jack/jack_module.cpp $(LDFLAGS)

ADSR.o:
	$(CC) -c $< $(CFLAGS) ADSR.cpp

MoogFilter.o:
	$(CC) -c $< $(CFLAGS) MoogFilter.cpp

Oscillator.o:
	$(CC) -c $< $(CFLAGS) oscillator.cpp

RtMidi.o:
	$(CC) -c $< -D__MACOSX_CORE__ $(CFLAGS) ./rtmidi-master/RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation

clean:
	rm -f *.o
	rm -f example

















#MidiSynth : synth.cpp
#		g++ -Wall -D__MACOSX_CORE__ -std=c++11 -ljack synth.cpp SynthClass.cpp oscillator.cpp ADSR.cpp  MoogFilter.cpp ./rtmidi-master/RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
#
#clean:
	rm -f *.o

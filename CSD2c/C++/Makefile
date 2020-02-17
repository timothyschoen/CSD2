CC = g++
CFLAGS = -Wall -std=c++17
LDFLAGS= -ljack

all: Halite

Halite : jack_module.o
	$(CC) -o $@ $(CFLAGS) main.cpp ./gen/phasor.cpp ./gen/genlib.cpp jack_module.o ./gen/json.o ./gen/json_builder.o $(LDFLAGS)
	mv Halite ../compiled/

jack_module.o:
	$(CC) -c $< $(CFLAGS) ./jack/jack_module.cpp $(LDFLAGS)

phasor.o: genlib.o
	$(CC) -o $@ $(CFLAGS) ./gen/phasor.cpp ./count.cpp genlib.o $(LDFLAGS)

genlib.o: json.o
	$(CC) -o $@ $(CFLAGS) ./gen/genlib.cpp ./gen/json.o ./gen/json_builder.o $(LDFLAGS)


json.o:
	gcc -c ./gen/json.o ./gen/json_builder.o

clean:
	rm -f *.o
	rm -f example
	make

cleanup:
	rm -f *.o
	rm -f *.wav
	rm -f example














#MidiSynth : synth.cpp
#		g++ -Wall -D__MACOSX_CORE__ -std=c++11 -ljack synth.cpp SynthClass.cpp oscillator.cpp ADSR.cpp  MoogFilter.cpp ./rtmidi-master/RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
#
#clean:
	rm -f *.o

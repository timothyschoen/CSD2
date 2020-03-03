CC = g++
CFLAGS = -Wall -std=c++17
LDFLAGS= -ljack
CAFLAGS= -D__MACOSX_CORE__ -framework CoreMIDI -framework CoreAudio -framework CoreFoundation -lpthread
#AFFLAGS= -I/opt/arrayfire/include -rpath /opt/arrayfire/lib -lafcpu -L/opt/arrayfire/lib -framework Accelerate
MKLFLAGS=  -lm -ldl -llapack


all: Halite

Halite : RtMidi.o, RtAudio.o genlib.o
	$(CC) -o $@ $(CFLAGS) $(CAFLAGS) main.cpp genlib.o phasor.o json.o json_builder.o RtMidi.o RtAudio.o $(MKLFLAGS)
	mv Halite ../compiled/


RtMidi.o, RtAudio.o:
	$(CC) -c $< $(CFLAGS) $(CAFLAGS) ./rtlibs/RtMidi.cpp ./rtlibs/RtAudio.cpp

genlib.o: json.o
	$(CC) -c $< $(CFLAGS) ./gen/genlib.cpp ./gen/phasor.cpp ./gen/json.o ./gen/json_builder.o

json.o:
	gcc -c ./gen/json.c ./gen/json_builder.c



clean:
	rm -f *.o
	rm -f example
	rm -f genHeaders.h
	make

cleanup:
	rm -f *.o
	rm -f *.wav
	rm -f genHeaders.h
	rm -f example

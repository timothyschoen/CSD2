CC = clang++
CFLAGS = -Wall -std=c++17 -lm
AUFLAGS= -lpthread
GENDEPS=

# Extra flags for different kinds of solvers
#AFFLAGS= -I/opt/arrayfire/include -rpath /opt/arrayfire/lib -lafcpu -L/opt/arrayfire/lib -framework Accelerate
#MKLFLAGS=  -lm -ldl -llapack


UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	AUFLAGS += -D__LINUX_ALSA__ -lasound
endif
ifeq ($(UNAME_S),Darwin)
	AUFLAGS += -D__MACOSX_CORE__ -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
	#GENDEPS += genlib.o phasor.o json.o json_builder.o     for gen support
endif



all: Halite

# Enable support for Max/MSP gen patchers (currently macOs only)

# ifeq ($(UNAME_S),Darwin)

#genlib.o: json.o
#	$(CC) -c $< $(CFLAGS) ./gen/genlib.cpp ./gen/phasor.cpp ./gen/json.o ./gen/json_builder.o

#json.o:
#	gcc -c ./gen/json.c ./gen/json_builder.c

#endif

Halite : RtMidi.o, RtAudio.o # $(GENDEPS) 
	$(CC) -o $@ $(CFLAGS) main.cpp RtMidi.o RtAudio.o $(AUFLAGS)
	mv Halite ../compiled/

RtMidi.o, RtAudio.o:
	$(CC) -c $< $(CFLAGS) $(AUFLAGS) ./rtlibs/RtMidi.cpp ./rtlibs/RtAudio.cpp 






clean:
	rm -f *.o
	rm -f example
	rm -f genHeaders.h
	make

cleanup:
	rm -f *.o
	rm -f *.wav
	rm -f ../saves/Examples/*.wav
	rm -f ../saves/*.wav
	rm -f genHeaders.h
	rm -f example

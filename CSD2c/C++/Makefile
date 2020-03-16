CC = clang++
CFLAGS = -Wall -std=c++17
AUFLAGS := -lpthread -lm
GENDEPS :=

# Extra flags for different kinds of solvers
#AFFLAGS= -I/opt/arrayfire/include -rpath /opt/arrayfire/lib -lafcpu -L/opt/arrayfire/lib -framework Accelerate
MKLFLAGS=  -ldl -llapack


UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	AUFLAGS += -D__LINUX_ALSA__ -lasound $(MKLFLAGS)
endif
ifeq ($(UNAME_S),Darwin)
	AUFLAGS += -D__MACOSX_CORE__ -framework CoreMIDI -framework CoreAudio -framework CoreFoundation $(MKLFLAGS)
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

Halite : RtMidi.o RtAudio.o MNASystem.o MNACell.o NetList.o MNASolver.o # $(GENDEPS)
	$(CC) -o $@ $(CFLAGS) main.cpp RtMidi.o RtAudio.o MNASystem.o MNACell.o NetList.o MNASolver.o $(AUFLAGS)
	mv Halite ../compiled/

RtMidi.o: #AUflags shouldn't be necessary but it won't work on mac without
	$(CC) -c -Wno-everything $(CFLAGS) $(AUFLAGS) ./rtlibs/RtMidi.cpp

RtAudio.o: # We mute the warnings that this library throws, which are all related to using -l to make a .o file (required by the library though)
	$(CC) -c -Wno-everything $(CFLAGS) $(AUFLAGS) ./rtlibs/RtAudio.cpp

NetList.o:
	$(CC) -c $(CFLAGS) NetList.cpp

MNASystem.o:
	$(CC) -c $(CFLAGS) MNASystem.cpp

MNACell.o:
	$(CC) -c $(CFLAGS) MNACell.cpp

MNASolver.o:
	$(CC) -c $(CFLAGS) MNASolver.cpp


clean:
	mkdir -p ../compiled
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

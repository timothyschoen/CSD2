CC = clang++
CFLAGS = -Wall -std=c++17
AUFLAGS =
MAINFLAGS = -lpthread -lm -llo

COMPONENTS = analogBasics.o analogJunctions.o analogIO.o analogInteraction.o digitalIO.o digitalTimeBased.o digitalSignalGenerators.o digitalUtility.o digitalCalculus.o domainConverters.o

# Extra flags for different kinds of solvers
#AFFLAGS= -I/opt/arrayfire/include -rpath /opt/arrayfire/lib -lafcpu -L/opt/arrayfire/lib -framework Accelerate
MKLFLAGS=  -ldl -llapack


UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	MAINFLAGS += -lasound $(MKLFLAGS)
	AUFLAGS += -D__LINUX_ALSA__
endif
ifeq ($(UNAME_S),Darwin)
	MAINFLAGS += -framework CoreMIDI -framework CoreAudio -framework CoreFoundation $(MKLFLAGS)
	AUFLAGS += -D__MACOSX_CORE__
endif


all: Halite

# Main

Halite : RtMidi.o RtAudio.o MNASystem.o MNACell.o NetList.o MNASolver.o $(COMPONENTS) # $(GENDEPS)
	$(CC) -o $@ $(CFLAGS) main.cpp RtMidi.o RtAudio.o MNASystem.o MNACell.o NetList.o MNASolver.o $(COMPONENTS) $(AUFLAGS) $(MAINFLAGS)
	mv Halite ../compiled/


# Libraries
RtMidi.o: #AUflags shouldn't be necessary but it won't work on mac without
	$(CC) -c $(CFLAGS) $(AUFLAGS) ./rtlibs/RtMidi.cpp

RtAudio.o: # We mute the warnings that this library throws, which are all related to using -l to make a .o file (required by the library though)
	$(CC) -c $(CFLAGS) $(AUFLAGS) ./rtlibs/RtAudio.cpp


# System

NetList.o:
	$(CC) -c $(CFLAGS) NetList.cpp

MNASystem.o:
	$(CC) -c $(CFLAGS) MNASystem.cpp

MNACell.o:
	$(CC) -c $(CFLAGS) MNACell.cpp

MNASolver.o:
	$(CC) -c $(CFLAGS) MNASolver.cpp



# Components


analogBasics.o:
	$(CC) -c $(CFLAGS) Components/analogBasics.cpp

analogJunctions.o:
	$(CC) -c $(CFLAGS) Components/analogJunctions.cpp

analogIO.o:
	$(CC) -c $(CFLAGS) Components/analogIO.cpp

analogInteraction.o:
	$(CC) -c $(CFLAGS) Components/analogInteraction.cpp

digitalUtility.o:
	$(CC) -c $(CFLAGS) Components/digitalUtility.cpp

digitalCalculus.o:
	$(CC) -c $(CFLAGS) Components/digitalCalculus.cpp

digitalIO.o:
	$(CC) -c $(CFLAGS) Components/digitalIO.cpp

digitalSignalGenerators.o:
	$(CC) -c $(CFLAGS) Components/digitalSignalGenerators.cpp

digitalTimeBased.o:
	$(CC) -c $(CFLAGS) Components/digitalTimeBased.cpp

domainConverters.o:
	$(CC) -c $(CFLAGS) Components/domainConverters.cpp



clean:
	mkdir -p ../compiled
	rm -f *.o
	rm -f example
	rm -f Components/*.o
	make

cleanup:
	rm -f *.o
	rm -f *.wav
	rm -f ../saves/Examples/*.wav
	rm -f ../saves/*.wav
	rm -f genHeaders.h
	rm -f example

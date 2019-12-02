

MidiInstrument : MidiInstrumentClass.o rtmidi.o MidiInstrument.cpp
	g++ -o MidiInstrument MidiInstrument.cpp MidiInstrumentClass.o

MidiInstrumentClass.o : rtmidi.o MidiInstrumentClass.h ./rtmidi-master/RtMidi.h
	g++ -Wall -D__MACOSX_CORE__ -c -o MidiInstrumentClass.o MidiInstrumentClass.h rtmidi.o

rtmidi.o : ./rtmidi-master/RtMidi.cpp ./rtmidi-master/RtMidi.h
	g++ -Wall -D__MACOSX_CORE__ -c -o rtmidi.o ./rtmidi-master/RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation


# g++ -Wall -D__MACOSX_CORE__ -o MidiInstrument MidiInstrument.cpp ./rtmidi/RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation

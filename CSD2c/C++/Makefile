CC = g++
CFLAGS = -Wall -std=c++17
# CAFLAGS
CAFLAGS = -D__LINUX_ALSA__ -lasound -lpthread

all: Halite

Halite : RtMidi.o
	$(CC) -o $@ $(CFLAGS)  main.cpp RtMidi.o RtAudio.o $(CAFLAGS)
	mv Halite ../compiled/


RtMidi.o:
	$(CC) -c $< $(CFLAGS) $(CAFLAGS) ./rtlibs/RtMidi.cpp ./rtlibs/RtAudio.cpp

#phasor.o: genlib.o
#	$(CC) -o $@ $(CFLAGS) ./gen/phasor.cpp ./count.cpp genlib.o $(LDFLAGS)


#genlib.o: json.o
#	$(CC) -o $@ $(CFLAGS) ./gen/genlib.cpp ./gen/json.o ./gen/json_builder.o $(LDFLAGS)


#genHeaders.h:
#	bash -c 'for i in `ls ./gen/*.h`; do echo "#include <$i>" >>genHeaders.h; done'

#json.o:
#	gcc -c ./gen/json.c ./gen/json_builder.c

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

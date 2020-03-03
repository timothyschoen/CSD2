CC = g++
CFLAGS = -Wall -std=c++17
LDFLAGS= -ljack
# CAFLAGS
CAFLAGS = -D__UNIX_JACK__

all: Halite

Halite : jack_module.o
	$(CC) -o $@ $(CFLAGS) main.cpp ./gen/phasor.cpp ./gen/genlib.cpp jack_module.o ./gen/json.o ./gen/json_builder.o $(LDFLAGS) # $(CAFLAGS)
	mv Halite ../compiled/

jack_module.o:
	$(CC) -c $< $(CFLAGS) ./jack/jack_module.cpp $(LDFLAGS)LDFLAGS

phasor.o: genlib.o
	$(CC) -o $@ $(CFLAGS) ./gen/phasor.cpp ./count.cpp genlib.o $(LDFLAGS)


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

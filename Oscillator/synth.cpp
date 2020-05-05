#include <unistd.h>
#include <iostream>
#include <thread>
#include "oscillator.h"
#include "MoogFilter.h"
#include "ADSR.h"
#include "Player.h"
#include "./jack/jack_module.h"
#include "./rtmidi-master/RtMidi.h"

// Select waveshape
int choice = 1;
//Variables to store user responses
char tempvar = 0;

//No. of voices
int voices = 4;
//Sequencer on/off
bool seqRunning = false;
bool running = true;
//Player object will make notes and rhythms for the sequencer
Player player;

//JackModule jack;

// Initialize Jack server


void keyinput()
{
        while(running) {
                std::cout << "What kind of waveshape do you want??" << std::endl << "1: Sine" << std::endl << "2: Saw" << std::endl << "3: Square" << std::endl << "4: Triangle" << std::endl;
                std::cout << "5: start/stop playing a sequence" << std::endl;
                std::cout << "q: quit" << std::endl;

                std::cin >> tempvar;
                // If the user chose a waveshape
                if (tempvar > '0' && tempvar <= '4' && std::cin) {
                        choice = (int)tempvar-48; //ASCII to number
                }
                // If the user chose to toggle the sequencer
                else if (tempvar == '5' && std::cin) {
                        if (seqRunning) {
                                seqRunning = false;
                        }
                        else {
                                seqRunning = true;
                        }
                }
                // Quitting
                else if (tempvar == 'q') {
                        running = false;
                }
                // If the response is invalid
                else {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                std::cout << std::endl << std::endl;
        }

}

int main(int argc,char **argv) {

        JackModule jack;
        jack.init("server 1");
        //Thread for user input
        std::thread inthread (keyinput);
        // fix use of 4... but how...!!!!!!!!!!!!!!!
        Oscillator oscs[4] {{jack.getSamplerate()}, {jack.getSamplerate()}, {jack.getSamplerate()}, {jack.getSamplerate()}};
        Oscillator lfo(1000); //Sets samplerate to controlrate

        // Moog ladder filter I found online
        MoogFilter filt1;
        // Init settings
        float filterpitch = 60;
        filt1.setPitch(filterpitch);
        filt1.setRes(0.2);

        // Envelope
        ADSR envelopes[4] {{20, 800, 0.3, 600}, {20, 800, 0.3, 600}, {20, 800, 0.3, 600}, {20, 800, 0.3, 600}};
        ADSR modenvelope(20, 800, 0.3, 600);

        int envelopeAmp = 0;
        int lfoAmp = 0;
        int lfoshape = 0;
        //Variable to store current sample
        double oscSample;
        // Function to execute for each sample
        jack.onProcess = [&oscs, &oscSample, &filt1, &envelopes](jack_default_audio_sample_t *inBuf,
                                                                 jack_default_audio_sample_t *outBuf, jack_nframes_t nframes)
                         {
                                 //loop through frames, retrieve sample of sine per frame
                                 for(int i = 0; i < nframes; i++) {
                                         oscSample = 0;
                                         //TODO check type of jack_default_audio_sample_t, double? or float?
                                         for(int x = 0; x < voices; x++) {
                                                 // Move oscillator  to next sample
                                                 oscs[x].tick();
                                                 // Retrieve sample at envelope's level
                                                 oscSample += oscs[x].getSample(choice-1, envelopes[x].getValue());
                                         }
                                         // Make softer and filter
                                         outBuf[i] = filt1.process(oscSample)*0.05;
                                 }
                                 return 0;
                         };
        jack.autoConnect();


        // Empty midi message
        std::vector<unsigned char> message;
        int nBytes;
        double stamp;

        int noteLast[voices];

        // Init values dependent of the number of voices FIX
        for (int i = 0; i < voices; i++) {
                noteLast[i] = 0;
        }

        // Start RtMidi and open virtual port
        RtMidiIn *midiin = new RtMidiIn();
        midiin->openVirtualPort("Input Port");
        midiin->ignoreTypes( false, false, false );

        // Loop
        while (running) {

                // See if there is a message
                stamp = midiin->getMessage( &message );

                // Check for a seqencer message
                // returns 0 if there's no new note
                int seqnote = player.getNote();
                int note;
                // If the sequencer returns a note
                if(seqRunning && seqnote != 0 && seqnote <= 127) {
                        // Write a midi message for our midi interpreter
                        message.push_back( 192 );
                        message.push_back( 5 );
                        message.push_back( 5 );
                        message[0] = 144;
                        message[1] = seqnote;
                        message[2] = 60;
                }
                // 128+notevalue indicates a note-off because returning arrays is complicated...
                if (seqnote >= 128) {
                        // Note-off message for our midi interpreter
                        message.push_back( 192 );
                        message.push_back( 5 );
                        message.push_back( 5 );
                        message[0] = 128;
                        message[1] = seqnote-128;
                        message[2] = 60;
                }
                //Check is there is any message from RtMidi or the sequencer
                nBytes = message.size();
                if ( nBytes > 0) {
                        // If it's a note-on
                        if ((int)message[0] == 144 && (int)message[2] != 0) {
                                note = (int)message[1];
                                // Variable to store the free voice's index
                                int freevoice = 3;
                                bool done = false;
                                // Two for loops to make sure that we first check if the note exist before we start looking for a free voice
                                for(int i = 0; i < voices; i++) {
                                        // If the note is already being played, use the same voice
                                        if(noteLast[i] == note) {
                                                freevoice = i;
                                                done = true;
                                                break;
                                        }
                                }
                                for(int i = 0; i < voices; i++) {
                                        // Check with the ADSR if the current voice has been released
                                        bool released = envelopes[i].getReleased();
                                        if(released == true && done == false) {
                                                freevoice = i;
                                                done = true;
                                                break;
                                        }
                                }
                                // Set the pitch of the right oscillator
                                oscs[freevoice].setPitch(note);

                                noteLast[freevoice] = note;
                                //Trigger the right envelope
                                envelopes[freevoice].noteOn();
                                modenvelope.noteOn();
                        }
                        // Process a note-off message
                        else if ((int)message[0] == 128 || ((int)message[2] == 0 && ((int)message[0] == 144))) {
                                for(int i = 0; i < voices; i++) {
                                        if(noteLast[i] == (int)message[1]) {
                                                noteLast[i] = 0;
                                                envelopes[i].noteOff();
                                        }
                                }
                        }
                        // Process MIDI CCs
                        else if ((int)message[0] == 176) {
                                // Midi CC1: Change filter cutoff
                                if ((int)message[1] == 0 && (int)message[2] < 110) {
                                        // setPitch is a part of Synthclass that will set the frequency with MtoF conversion
                                        filterpitch = (float)message[2];
                                }
                                // Set the filter resonance
                                else if ((int)message[1] == 1) {
                                        filt1.setRes((float)message[2]/127);
                                }
                                // Set envelope attack, decay, sustain and release times
                                else if ((int)message[1] == 2) {
                                        for (int i = 0; i < voices; i++) envelopes[i].setAttack((float)message[2]*30);
                                }
                                else if ((int)message[1] == 3) {
                                        for (int i = 0; i < voices; i++) envelopes[i].setDecay((float)message[2]*30);
                                }
                                else if ((int)message[1] == 4) {
                                        for (int i = 0; i < voices; i++) envelopes[i].setSustain((float)message[2]/127);
                                }
                                else if ((int)message[1] == 5) {
                                        for (int i = 0; i < voices; i++) envelopes[i].setRelease((float)message[2]*30);
                                }
                                // Functionaliteit toevoegen!!
                                else if ((int)message[1] == 6) {
                                        lfo.setPitch((float)message[2]/12.7);
                                }
                                else if ((int)message[1] == 7) {
                                        lfoAmp = (float)message[2]/2;
                                }
                                else if ((int)message[1] == 8) {
                                        lfoshape = (int)message[2] % 4;
                                }

                                else if ((int)message[1] == 9) {
                                        modenvelope.setAttack((float)message[2]*30);
                                }
                                else if ((int)message[1] == 10) {
                                        modenvelope.setDecay((float)message[2]*30);
                                }
                                else if ((int)message[1] == 11) {
                                        modenvelope.setSustain((float)message[2]/127);
                                }
                                else if ((int)message[1] == 12) {
                                        modenvelope.setRelease((float)message[2]*30);
                                }
                                else if ((int)message[1] == 13) {
                                        envelopeAmp = (float)message[2];
                                }



                        }
                }

                // Sleep for 1000 microseconds = 1ms
                usleep(1000);
        }
        //note-offs before quitting
        for (int i = 0; i < voices; i++) {
                envelopes[i].noteOff();
                modenvelope.noteOff();
        }
        usleep(1000);
        jack.end();
}

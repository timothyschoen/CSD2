#include <unistd.h>
#include <iostream>
#include <cmath>
#include "Synth.h"


//Constructor
Synth::Synth() {
    filt1.setPitch(20);
    filt1.setRes(0.2);

    // Initialize thead for control-rate stuff
    controlThread = std::thread(&Synth::controlFunc, this);

}

//Destructor
Synth::~Synth() {
    running = false;
    controlThread.join();
}


void Synth::noteOn(int note) {
    bool done = false;
      // Two for loops to make sure that we first check if the note exist before we start looking for a free voice
      for(int i = 0; i < voices; i++){
          // If the note is already being played, use the same voice
          if(noteLast[i] == note){
            freevoice = i;
            done = true;
            break;
          }
        }
        for(int i = 0; i < voices; i++){
         // Check with the ADSR if the current voice has been released
         bool released = envelopes[i].getReleased();
         if(released == true && done == false){
           freevoice = i;
           done = true;
           break;
      }
    }

     // Set the pitch of the right oscillator
     polyosc1[freevoice].setPitch(note);
     polyosc2[freevoice].setPitch(note);

     noteLast[freevoice] = note;
     //Trigger the right envelope
     envelopes[freevoice].noteOn();
     modenvelope.noteOn();
                   
}
 
//Constructor
void Synth::noteOff(int note) {
       
       for(int i = 0; i < voices; i++){
       if(noteLast[i] == note){
         noteLast[i] = 0;
         envelopes[i].noteOff();
        }
    }
}

//Constructor
double Synth::getSample() {
    double oscSample = 0;
    for(int x = 0; x < voices; x++) {
        // Move oscillator  to next sample
        polyosc1[x].tick();
        polyosc2[x].tick();
        // Retrieve sample at envelope's level
        oscSample += polyosc1[x].getSample(envelopes[x].getSample(1)*0.5);
        oscSample += polyosc2[x].getSample(envelopes[x].getSample(1)*0.5);
    }
    // Make softer and filter
    return filt1.getSample(oscSample)*0.05;
}



void Synth::setValuePointer(double* valuepointer)
{
    sliderValues = valuepointer;
}

void Synth::updateControls()
{
    // There's a glitch in MacOS JUCE (maybe just Catalina)
    // It doesn't allow calling any method function from the slider move callback
    // This is a dirty workaround

      filterpitch = sliderValues[0];
      filt1.setRes(sliderValues[1]);
      for (int i = 0; i < voices; i++) envelopes[i].setAttack(sliderValues[2]);
      for (int i = 0; i < voices; i++) envelopes[i].setDecay(sliderValues[3]);
      for (int i = 0; i < voices; i++) envelopes[i].setSustain(sliderValues[4]);
      for (int i = 0; i < voices; i++) envelopes[i].setRelease(sliderValues[5]);
      lfo.setPitch(sliderValues[6]);
      lfoAmp = sliderValues[7];
      modenvelope.setAttack(sliderValues[8]);
      modenvelope.setDecay(sliderValues[9]);
      modenvelope.setSustain(sliderValues[10]);
      modenvelope.setRelease(sliderValues[11]);
      envelopeAmp = sliderValues[12];
      for (int i = 0; i < voices; i++) polyosc1[i].setTranspose(sliderValues[13]);

      for (int i = 0; i < voices; i++) polyosc2[i].setTranspose(sliderValues[14]);
      for (int i = 0; i < voices; i++) polyosc1[i].setShape(sliderValues[17]);
      for (int i = 0; i < voices; i++) polyosc2[i].setShape(sliderValues[18]);
      lfo.setShape(sliderValues[19]);

    }

void Synth::controlFunc()
{
    while(running) {
    updateControls();
    // Tick the envelopes and lfos once every millisecond (our control rate, audio rate is not necessary)
    for(int i = 0; i < voices; i++){
      envelopes[i].tick();
    }
    modenvelope.tick();
    lfo.tick();
    
    int cutoff = filterpitch+(modenvelope.getSample(1)*envelopeAmp)+(lfo.getSample(1.)*lfoAmp);
    // Above 105 causes crash
    cutoff = std::min(cutoff, 100);
    filt1.setPitch(cutoff);
        
    usleep(1000);
    }

}

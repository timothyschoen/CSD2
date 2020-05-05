/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include <thread>
#include <unistd.h>


#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ADSR.h"
#include "oscillator.h"
#include "MoogFilter.h"

int voices = 4;

Oscillator polyosc1[4] {{44100}, {44100}, {44100}, {44100}};
Oscillator polyosc2[4] {{44100}, {44100}, {44100}, {44100}};

Oscillator lfo(1000); //Sets samplerate to controlrate

  // Moog ladder filter
  MoogFilter filt1;
  // Init settings
  float filterpitch = 60;
float envelopeAmp = 0;
float lfoAmp = 0;


  // Envelope
  Envelope envelopes[4] {{20, 800, 0.3, 600}, {20, 800, 0.3, 600}, {20, 800, 0.3, 600}, {20, 800, 0.3, 600}};
  Envelope modenvelope(20, 800, 0.3, 600);

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================

void NewProjectAudioProcessor::setSlider (int index, double value)
{
    switch(index) {
        case 0:
            filterpitch = value;
        break;
        case 1:
            filt1.setRes(value);
        break;
        case 2:
            for (int i = 0; i < voices; i++) envelopes[i].setAttack(value);
        break;
        case 3:
            for (int i = 0; i < voices; i++) envelopes[i].setDecay(value);
        break;
        case 4:
            for (int i = 0; i < voices; i++) envelopes[i].setSustain(value);
        break;
        case 5:
            for (int i = 0; i < voices; i++) envelopes[i].setRelease(value);
        break;
        case 6:
            lfo.setPitch(value);
        break;
        case 7:
            lfoAmp = value;
        break;
        case 8:
            modenvelope.setAttack(value);
        break;
        case 9:
            modenvelope.setDecay(value);
        break;
        case 10:
            modenvelope.setSustain(value);
        break;
        case 11:
            modenvelope.setRelease(value);
        break;
        case 12:
            envelopeAmp = value;
        break;
        case 13:
            for (int i = 0; i < voices; i++) polyosc1[i].setTranspose(value);
        break;
        case 14:
            for (int i = 0; i < voices; i++) polyosc2[i].setTranspose(value);
        break;
        case 17:
            for (int i = 0; i < voices; i++) polyosc1[i].setShape(value);
        break;
        case 18:
            for (int i = 0; i < voices; i++) polyosc2[i].setShape(value);
        break;
        case 19:
            lfo.setShape(value);
        break;
    }
}



//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    filt1.setPitch(filterpitch);
    filt1.setRes(0.2);
    
    // Initialize thead for control-rate stuff
    controlThread = std::thread(&NewProjectAudioProcessor::controlFunc, this);
    

    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void NewProjectAudioProcessor::releaseResources()
{
    //controlThread.join();
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


int note = 60;
int freevoice = 3;
int noteLast[4] = {0, 0, 0, 0};


void NewProjectAudioProcessor::controlFunc()
{
    while(true) {
    // Tick the envelopes and lfos once every millisecond (our control rate, audio rate is not necessary)
    for(int i = 0; i < voices; i++){
      envelopes[i].tick();

    }
    modenvelope.tick();
    lfo.tick();
    
        int cutoff = filterpitch+(modenvelope.getValue()*envelopeAmp)+(lfo.getSample(1.)*lfoAmp);
    // Above 105 causes crash
    cutoff = std::min(cutoff, 100);
    filt1.setPitch(cutoff);
        usleep(1000);
    }

}


void NewProjectAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    const float* inBuffer = buffer.getReadPointer (0);
    float* outBufferL = buffer.getWritePointer (0);
    float* outBufferR = buffer.getWritePointer (1);
    


       int time;
    
       MidiMessage m;
       
       for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
       {
           if (m.isNoteOn() && m.getVelocity() != 0)
           {
               //uint8 newVel = (uint8)noteOnVel;
               //m = MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), newVel);
               
               note = m.getNoteNumber();
               
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
           else if (m.isNoteOff() || m.getVelocity() != 0)
           {
               note = m.getNoteNumber();
               for(int i = 0; i < voices; i++){
               if(noteLast[i] == note){
                 noteLast[i] = 0;
                 envelopes[i].noteOff();
                }
            }
           }
           else if (m.isAftertouch())
           {
           }
           else if (m.isPitchWheel())
           {
           }
    
       }

        



    for (int i=0; i<buffer.getNumSamples(); ++i)
    {
        
      oscSample = 0;
      //TODO check type of jack_default_audio_sample_t, double? or float?
      for(int x = 0; x < voices; x++) {
        // Move oscillator  to next sample
        polyosc1[x].tick();
        polyosc2[x].tick();
        // Retrieve sample at envelope's level
          oscSample += polyosc1[x].getSample(envelopes[x].getValue()*0.5);
          oscSample += polyosc2[x].getSample(envelopes[x].getValue()*0.5);
      }
      // Make softer and filter
      double outSample = filt1.process(oscSample)*0.05;
        outBufferL[i] = outSample;
        outBufferR[i] = outSample;
    }
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}

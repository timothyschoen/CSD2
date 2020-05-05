/*
   ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

   ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SliderClass.h"

//==============================================================================
/**
 */
class NewProjectAudioProcessorEditor : public AudioProcessorEditor, MidiKeyboardStateListener
{
public:
NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
~NewProjectAudioProcessorEditor();

//==============================================================================
void paint (Graphics&) override;
void resized() override;

private:
    
MidiKeyboardState keyboardState;
MidiKeyboardComponent keyboardComponent;

    
// This reference is provided as a quick way for your editor to
// access the processor object that created it.
NewProjectAudioProcessor& processor;

SliderClass sliders;

void handleNoteOn(MidiKeyboardState * keyState, int midiChannel, int midiNoteNumber,float velocity) override;

void handleNoteOff(MidiKeyboardState * keyState, int midiChannel, int midiNoteNumber,float velocity) override;


JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};

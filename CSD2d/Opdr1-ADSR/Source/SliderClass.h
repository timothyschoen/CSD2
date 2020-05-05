//
//  SliderClass.hpp
//  VOX Wah
//
//  Created by Timothy Schoen on 01/05/2020.
//  Copyright © 2020 TS. All rights reserved.

#pragma once

#include <stdio.h>
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 */
class SliderClass : private Slider::Listener
{
public:


Slider sliders[15];
    
TextButton toggle[12];

SliderClass ();
~SliderClass();
    
MidiKeyboardState keyboardState;
MidiKeyboardComponent keyboardComponent;

NewProjectAudioProcessor* p;

void updateToggleState(NewProjectAudioProcessor &process, Button* button, int idx);
    
enum RadioButtonIds
{
    Shape1 = 1001,
    Shape2 = 1234,
    ShapeLFO = 5678
};
    
void makeSliders (AudioProcessorEditor &process, NewProjectAudioProcessor &processor);
void resized(AudioProcessorEditor &process);

void draw(Graphics &g, AudioProcessorEditor &process);


private:

void sliderValueChanged (Slider* slider) override;

};

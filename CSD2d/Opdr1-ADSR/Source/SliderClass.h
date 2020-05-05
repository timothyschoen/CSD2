//
//  SliderClass.hpp
//  VOX Wah
//
//  Created by Timothy Schoen on 01/05/2020.
//  Copyright © 2020 TS. All rights reserved.

#pragma once

#include <stdio.h>
#include <JuceHeader.h>
#include "Synth.h"
#include "PluginProcessor.h"

//==============================================================================
/**
 */
class SliderClass : private Slider::Listener
{
public:


SliderClass ();
~SliderClass();


void makeSliders (AudioProcessorEditor &process, NewProjectAudioProcessor &processor);

void resized(AudioProcessorEditor &process);

void draw(Graphics &g, AudioProcessorEditor &process);
    
void setValuePointer(double* vpointer);


private:
    
Slider sliders[15];
    
TextButton toggle[12];
    
NewProjectAudioProcessor* p;

double* sliderValues;
    
void updateToggleState(NewProjectAudioProcessor &process, Button* button, int idx);

enum RadioButtonIds
{
    Shape1 = 1001,
    Shape2 = 1234,
    ShapeLFO = 5678
};

void sliderValueChanged (Slider* slider) override;

};

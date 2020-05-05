/*
   ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

   ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SliderClass.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
        : AudioProcessorEditor (&p), processor (p)
{
        // Make sure that before the constructor has finished, you've set the
        // editor's size to whatever you need it to be.
        setSize (980, 450);
        sliders.makeSliders(*this, processor);
        processor.setValuePointer(sliders.getValuePointer());
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (Graphics& g)
{

        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));


        g.setColour (Colours::white);
        sliders.draw(g, *this);
}

void NewProjectAudioProcessorEditor::resized()
{
        sliders.resized(*this);

}

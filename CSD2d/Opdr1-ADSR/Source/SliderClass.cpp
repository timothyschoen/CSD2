#include "SliderClass.h"

SliderClass::SliderClass () : keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard)
 {};
SliderClass::~SliderClass() {};

void SliderClass::updateToggleState (NewProjectAudioProcessor &processor, Button* button, int idx)
{
    if (idx <= 4) {
        processor.setSlider(17, idx-1);
    }
    else if (idx >= 5 && idx < 9) {
        processor.setSlider(18, idx-5);
    }
    else {
        processor.setSlider(19, idx-9); 
    }

}

void SliderClass::makeSliders(AudioProcessorEditor &process, NewProjectAudioProcessor &processor)
{

        // TODO maak shape selector class
    for(int i = 0; i < 12; i++) {
        toggle[i].setClickingTogglesState (true);
        process.addAndMakeVisible (toggle[i]);
    }
    
    process.addAndMakeVisible (keyboardComponent);
    
    toggle[0].setRadioGroupId(Shape1);
    toggle[1].setRadioGroupId(Shape1);
    toggle[2].setRadioGroupId(Shape1);
    toggle[3].setRadioGroupId(Shape1);
    
    toggle[4].setRadioGroupId(Shape2);
    toggle[5].setRadioGroupId(Shape2);
    toggle[6].setRadioGroupId(Shape2);
    toggle[7].setRadioGroupId(Shape2);
    
    toggle[8].setRadioGroupId(ShapeLFO);
    toggle[9].setRadioGroupId(ShapeLFO);
    toggle[10].setRadioGroupId(ShapeLFO);
    toggle[11].setRadioGroupId(ShapeLFO);
    
    
    // Make the buttons look nice
    int edges[4] = {2, 3, 3, 1};
    
    for (int i = 0; i < 12; i++)
        toggle[i].setConnectedEdges(edges[i%4]);
    
    
    toggle[0].setButtonText("Sine");
    toggle[1].setButtonText ("Saw");
    toggle[2].setButtonText ("Square");
    toggle[3].setButtonText ("Triangle");
    
    toggle[4].setButtonText("Sine");
    toggle[5].setButtonText ("Saw");
    toggle[6].setButtonText ("Square");
    toggle[7].setButtonText ("Triangle");
    
    toggle[8].setButtonText("Sine");
    toggle[9].setButtonText ("Saw");
    toggle[10].setButtonText ("Square");
    toggle[11].setButtonText ("Triangle");
    
    
    toggle[0].onClick = [this, &processor] { updateToggleState (processor, &toggle[0], 1); };
    toggle[1].onClick = [this, &processor] { updateToggleState (processor, &toggle[1],   2);   };
    toggle[2].onClick = [this, &processor] { updateToggleState (processor, &toggle[2],  3);  };
    toggle[3].onClick = [this, &processor] { updateToggleState (processor, &toggle[3],  4);  };
    
    toggle[4].onClick = [this, &processor] { updateToggleState (processor, &toggle[4], 5); };
    toggle[5].onClick = [this, &processor] { updateToggleState (processor, &toggle[5],   6);   };
    toggle[6].onClick = [this, &processor] { updateToggleState (processor, &toggle[6],  7);  };
    toggle[7].onClick = [this, &processor] { updateToggleState (processor, &toggle[7],  8);  };
    
    toggle[8].onClick = [this, &processor] { updateToggleState (processor, &toggle[8], 9); };
    toggle[9].onClick = [this, &processor] { updateToggleState (processor, &toggle[9],   10);   };
    toggle[10].onClick = [this, &processor] { updateToggleState (processor, &toggle[10],  11);  };
    toggle[11].onClick = [this, &processor] { updateToggleState (processor, &toggle[11],  12);  };

    
    sliders[0].setRange(0., 127., 0.1);
    sliders[0].setTextValueSuffix (" Cutoff");
    sliders[0].setValue(84.);
    
    sliders[1].setRange(0., 1., 0.01);
    sliders[1].setTextValueSuffix (" Resonance");
    sliders[1].setValue(0.3);

    
    sliders[2].setRange(0., 3000, 1);
    sliders[2].setTextValueSuffix (" Attack");
    sliders[2].setValue(50);

    sliders[3].setRange(0., 3000, 1);
    sliders[3].setTextValueSuffix (" Decay");
    sliders[3].setValue(1000);

    sliders[4].setRange(0., 1.1, 0.1);
    sliders[4].setTextValueSuffix (" Sustain");
    sliders[4].setValue(0.2);
    
    sliders[5].setRange(0., 4000, 0.1);
    sliders[5].setTextValueSuffix (" Release");
    sliders[5].setValue(1000);

    sliders[6].setRange(-30., 4., 0.1);
    sliders[6].setTextValueSuffix (" LFO Rate");
    sliders[6].setValue(-20);
    
    sliders[7].setRange(0., 127., 0.1);
    sliders[7].setTextValueSuffix (" LFO->FILTER");
    sliders[7].setValue(0.2);

    sliders[8].setRange(0., 5000., 0.1);
    sliders[8].setTextValueSuffix (" Attack");
    sliders[8].setValue(50);

    sliders[9].setRange(0., 3000., 0.1);
    sliders[9].setTextValueSuffix (" Decay");
    sliders[9].setValue(30);
    
    sliders[10].setRange(0., 1., 0.01);
    sliders[10].setTextValueSuffix (" Sustain");
    sliders[10].setValue(0.2);

    sliders[11].setRange(0., 5000., 0.1);
    sliders[11].setTextValueSuffix (" Release");
    sliders[11].setValue(100);

    sliders[12].setRange(-127., 127., 0.1);
    sliders[12].setTextValueSuffix (" ENV->FILTER");
    sliders[12].setValue(5);
    
    sliders[13].setRange(-24., 24, 0.1);
    sliders[13].setTextValueSuffix ("OSC1 Pitch");
    sliders[13].setValue(0);
    
    sliders[14].setRange(-24., 24, 0.1);
    sliders[14].setTextValueSuffix ("OSC2 Pitch");
    sliders[14].setValue(0);

    
    for (int i = 0; i < 15; i++) {
        sliders[i].setSliderStyle (Slider::LinearHorizontal);
        sliders[i].setPopupDisplayEnabled (false, false, &process);
        sliders[i].setTextBoxStyle (Slider::NoTextBox, false, 90, 0);
        sliders[i].addListener (this);
        process.addAndMakeVisible(sliders[i]);
    }
  }


void SliderClass::resized(AudioProcessorEditor &process)
{
    toggle[0].setBounds (80, 1*30, process.getWidth()/16, 20);
    toggle[1].setBounds (140, 1*30, process.getWidth()/16, 20);
    toggle[2].setBounds (200, 1*30, process.getWidth()/16, 20);
    toggle[3].setBounds (260, 1*30, process.getWidth()/16, 20);
    
    toggle[4].setBounds (80, 4*30, process.getWidth()/16, 20);
    toggle[5].setBounds (140, 4*30, process.getWidth()/16, 20);
    toggle[6].setBounds (200, 4*30, process.getWidth()/16, 20);
    toggle[7].setBounds (260, 4*30, process.getWidth()/16, 20);
    
    toggle[8].setBounds (400, 8*30, process.getWidth()/16, 20);
    toggle[9].setBounds (460, 8*30, process.getWidth()/16, 20);
    toggle[10].setBounds (520, 8*30, process.getWidth()/16, 20);
    toggle[11].setBounds (580, 8*30, process.getWidth()/16, 20);
    
    sliders[0].setBounds (80, 7*30, process.getWidth()/4, 30);
    sliders[1].setBounds (80, 8*30, process.getWidth()/4, 30);
    
    
    sliders[12].setBounds (80, 10*30, process.getWidth()/4, 30);
    
    sliders[2].setBounds (400, 1*30, process.getWidth()/4, 30);
    sliders[3].setBounds (400, 2*30, process.getWidth()/4, 30);
    sliders[4].setBounds (400, 3*30, process.getWidth()/4, 30);
    sliders[5].setBounds (400, 4*30, process.getWidth()/4, 30);
    
    sliders[6].setBounds (400, 9*30, process.getWidth()/4, 30);
    sliders[7].setBounds (80, 9*30, process.getWidth()/4, 30);
    
    sliders[8].setBounds (720, 1*30, process.getWidth()/4, 30);
    sliders[9].setBounds (720, 2*30, process.getWidth()/4, 30);
    sliders[10].setBounds (720, 3*30, process.getWidth()/4, 30);
    sliders[11].setBounds (720, 4*30, process.getWidth()/4, 30);
    
    sliders[13].setBounds (80, 2*30, process.getWidth()/4, 30);
    sliders[14].setBounds (80, 5*30, process.getWidth()/4, 30);

}


void SliderClass::draw(Graphics &g, AudioProcessorEditor &process)
{
    g.drawFittedText ("Shape", 20, 1*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("Pitch", 20, 2*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("Shape", 20, 4*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("Pitch", 20, 5*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("Cutoff", 20, 7*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("Resonance", 20, 8*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("LFO->Filter", 20, 9*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("ENV->Filter", 20, 10*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("Attack", 340, 1*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("Decay", 340, 2*30, process.getWidth()-30, 30, Justification::left, 1);

    g.drawFittedText ("Sustain", 340, 3*30, process.getWidth()-30, 30, Justification::left, 1);

    g.drawFittedText ("Release", 340, 4*30, process.getWidth()-30, 30, Justification::left, 1);
    
    
    g.drawFittedText ("LFO Shape", 340, 8*30, process.getWidth()-30, 30, Justification::left, 1);

    g.drawFittedText ("LFO Rate", 340, 9*30, process.getWidth()-30, 30, Justification::left, 1);
    
    
    g.drawFittedText ("Attack", 670, 1*30, process.getWidth()-30, 30, Justification::left, 1);
    
    g.drawFittedText ("Decay", 670, 2*30, process.getWidth()-30, 30, Justification::left, 1);

    g.drawFittedText ("Sustain", 670, 3*30, process.getWidth()-30, 30, Justification::left, 1);

    g.drawFittedText ("Release", 670, 4*30, process.getWidth()-30, 30, Justification::left, 1);
    
    
}

double* SliderClass::getValuePointer ()
{
    return sliderValues;
}



void SliderClass::sliderValueChanged (Slider* slider)
{
  for (int i = 0; i < 16; i++) {
  if (slider == &sliders[i]) {
    sliderValues[i] = sliders[i].getValue();
    break;
    }
  }
}

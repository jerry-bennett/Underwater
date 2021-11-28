/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
using namespace std;
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class UnderwaterAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    UnderwaterAudioProcessorEditor (UnderwaterAudioProcessor&);
    ~UnderwaterAudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    std::vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>> sliderAttachment;
    
    Slider pitchKnob;
    
    Slider gainKnob;
    
    UnderwaterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnderwaterAudioProcessorEditor)
};

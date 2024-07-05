/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (DattorroReverbAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DattorroReverbAudioProcessor& audioProcessor;

    Slider predelayTimeSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> predelayTimeAttachment;
    juce::Label  predelayLabel;

    Slider decaySlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    juce::Label  decayLabel;

    Slider decayDif1Slider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> decayDif1Attachment;
    juce::Label  decayDif1Label;

    Slider inputDif1Slider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> inputDif1Attachment;
    juce::Label  inputDif1Label;

    Slider inputDif2Slider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> inputDif2Attachment;
    juce::Label  inputDif2Label;

    Slider bandwidthSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> bandwidthAttachment;
    juce::Label  bandwidthLabel;

    Slider dampingSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dampingAttachment;
    juce::Label  dampingLabel;

    Slider mixSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    juce::Label  mixLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (DattorroReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    addAndMakeVisible(predelayTimeSlider);
    predelayTimeAttachment =
        std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvst, "predelay", predelayTimeSlider);

    addAndMakeVisible(predelayLabel);
    predelayLabel.setText("Predelay Time", juce::dontSendNotification);
    predelayLabel.attachToComponent(&predelayTimeSlider, true);

    addAndMakeVisible(decaySlider);
    decayAttachment =
        std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvst, "decay", decaySlider);

    addAndMakeVisible(decayLabel);
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, true);

    addAndMakeVisible(decayDif1Slider);
    decayDif1Attachment =
        std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvst, "decayDif1", decayDif1Slider);

    addAndMakeVisible(decayDif1Label);
    decayDif1Label.setText("Decay Diffusion", juce::dontSendNotification);
    decayDif1Label.attachToComponent(&decayDif1Slider, true);

    addAndMakeVisible(inputDif1Slider);
    inputDif1Attachment =
        std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvst, "inputDif1", inputDif1Slider);

    addAndMakeVisible(inputDif1Label);
    inputDif1Label.setText("Input Diffusion 1", juce::dontSendNotification);
    inputDif1Label.attachToComponent(&inputDif1Slider, true);

    addAndMakeVisible(inputDif2Slider);
    inputDif2Attachment =
        std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvst, "inputDif2", inputDif2Slider);

    addAndMakeVisible(inputDif2Label);
    inputDif2Label.setText("Input Diffusion 2", juce::dontSendNotification);
    inputDif2Label.attachToComponent(&inputDif2Slider, true);

    addAndMakeVisible(bandwidthSlider);
    bandwidthAttachment =
        std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvst, "bandwidth", bandwidthSlider);

    addAndMakeVisible(bandwidthLabel);
    bandwidthLabel.setText("Bandwidth", juce::dontSendNotification);
    bandwidthLabel.attachToComponent(&bandwidthSlider, true);

    addAndMakeVisible(dampingSlider);
    dampingAttachment =
        std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvst, "damping", dampingSlider);

    addAndMakeVisible(dampingLabel);
    dampingLabel.setText("Damping", juce::dontSendNotification);
    dampingLabel.attachToComponent(&dampingSlider, true);

    addAndMakeVisible(mixSlider);
    mixAttachment =
        std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvst, "mix", mixSlider);

    addAndMakeVisible(mixLabel);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, true);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto b = getLocalBounds().removeFromRight(300);

    predelayTimeSlider.setBounds(b.removeFromTop(30));
    decaySlider.setBounds(b.removeFromTop(30));
    decayDif1Slider.setBounds(b.removeFromTop(30));
    inputDif1Slider.setBounds(b.removeFromTop(30));
    inputDif2Slider.setBounds(b.removeFromTop(30));
    bandwidthSlider.setBounds(b.removeFromTop(30));
    dampingSlider.setBounds(b.removeFromTop(30));
    mixSlider.setBounds(b.removeFromTop(30));
}

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DattorroReverbAudioProcessor::DattorroReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
,apvst(*this, nullptr, "ValueTree", createPararmeterLayout())
{
}

DattorroReverbAudioProcessor::~DattorroReverbAudioProcessor()
{
}

//==============================================================================
const juce::String DattorroReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DattorroReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DattorroReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DattorroReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DattorroReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DattorroReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DattorroReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DattorroReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DattorroReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void DattorroReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DattorroReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    plateReverb.prepareToPlay(sampleRate);
    
}

void DattorroReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DattorroReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void DattorroReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    plateReverb.processBlock (buffer, buffer.getNumSamples(), totalNumOutputChannels);

    plateReverb.setPredelayTime(apvst.getParameter("predelay")->getValue());
    plateReverb.setDecay(apvst.getParameter("decay")->getValue());
    plateReverb.setDecayDiffusion1(apvst.getParameter("decayDif1")->getValue());
    plateReverb.setInputDiffusion1(apvst.getParameter("inputDif1")->getValue());
    plateReverb.setInputDiffusion2(apvst.getParameter("inputDif2")->getValue());
    plateReverb.setBandwidth(apvst.getParameter("bandwidth")->getValue());
    plateReverb.setDamping(apvst.getParameter("damping")->getValue());
    plateReverb.setMix(apvst.getParameter("mix")->getValue());
}

//==============================================================================
bool DattorroReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DattorroReverbAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void DattorroReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = apvst.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DattorroReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvst.state.getType()))
            apvst.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
 
    return new DattorroReverbAudioProcessor();
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout DattorroReverbAudioProcessor::createPararmeterLayout()
{
    AudioProcessorValueTreeState::ParameterLayout parameterLayout;

    parameterLayout.add(
        std::make_unique<AudioParameterInt>
        ("predelay", "Predelay", 0, 1000, 0)
    );

    parameterLayout.add(
        std::make_unique<AudioParameterFloat>
        ("decay", "Decay", NormalisableRange<float>(0.0, 1.0), 0.5)
    );

    parameterLayout.add(
        std::make_unique<AudioParameterFloat>
        ("decayDif1", "Decay Diffusion", NormalisableRange<float>(0.0, 1.0), 0.7)
    );

    parameterLayout.add(
        std::make_unique<AudioParameterFloat>
        ("inputDif1", "Input Diffusion 1", NormalisableRange<float>(0.0, 1.0), 0.75)
    );

    parameterLayout.add(
        std::make_unique<AudioParameterFloat>
        ("inputDif2", "Input Diffusion 2", NormalisableRange<float>(0.0, 1.0), 0.625)
    );

    parameterLayout.add(
        std::make_unique<AudioParameterFloat>
        ("bandwidth", "Bandwidth", NormalisableRange<float>(0.0, 1.0), 0.9995)
    );

    parameterLayout.add(
        std::make_unique<AudioParameterFloat>
        ("damping", "Damping", NormalisableRange<float>(0.0, 1.0), 0.0005)
    );

    parameterLayout.add(
        std::make_unique<AudioParameterFloat>
        ("mix", "Mix", NormalisableRange<float>(0.0, 1.0), 0.5)
    );


    return parameterLayout;
}

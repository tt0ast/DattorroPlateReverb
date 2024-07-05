/*
  ==============================================================================

    PlateReverb.h
    Created: 17 May 2024 12:00:32pm
    Author:  Till

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DelayLine.h"

class PlateReverb
{
public:
    PlateReverb();
    
    void prepareToPlay (int sampleRate);

    void processBlock (juce::AudioBuffer<float>& buffer, int numSamples, int numChannels);

private:
    float calculateLattice (float sample, float coefficient, DelayLine& delayLine);

    float calculateReverseLattice (float sample, float coefficient, DelayLine& delayLine);

    float calculateOnepole (float sample, float coefficient, DelayLine& delayLine);

    float processDelay (float sample, DelayLine& delayLine);

    // parameters
    int predelayTime;
    float decay;
    float decayDiffusion1;
    float decayDiffusion2;
    float inputDiffusion1;
    float inputDiffusion2;
    float bandwidth;
    float damping;
    float mix;

    // hash maps to store delay times
    HashMap<String, float> delayTimesMilsec;
    HashMap<String, int> delayTimesSamp;

    // all delay lines
    DelayLine predelay;

    DelayLine delayLeft1;
    DelayLine delayLeft2;
    DelayLine delayRight1;
    DelayLine delayRight2;
    
    DelayLine bandwidthOnepole;
    DelayLine dampingOnepoleLeft;
    DelayLine dampingOnepoleRight;

    DelayLine inputDiffusion1A;
    DelayLine inputDiffusion1B;
    DelayLine inputDiffusion2A;
    DelayLine inputDiffusion2B;
    DelayLine decayDiffusion1L;
    DelayLine decayDiffusion2L;
    DelayLine decayDiffusion1R;
    DelayLine decayDiffusion2R;
};

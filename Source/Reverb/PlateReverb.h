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
    
    void prepareToPlay (double sampleRate);

    void processBlock (juce::AudioBuffer<float>& buffer, int numSamples, int numChannels);

    // setter functions for gui tests

    void setPredelayTime (int newPredelayTime);

    void setSize (float newDecay);

    void setHighCut (float newBandwidth);

    void setDamping (float newDamping);

    void setMix (float newMix);

private:
    float calculateLattice (float sample, float coefficient, DelayLine& delayLine);

    float calculateReverseLattice (float sample, float coefficient, DelayLine& delayLine);

    float calculateOnepole (float sample, float coefficient, DelayLine& delayLine);

    float processDelay (float sample, DelayLine& delayLine);

    int clamp (int low, int high, int value);

    float clamp (float low, float high,  float value);

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

    // delay times in samples for output and predelay
    int samplesPredelayTap;

    int samplesDelayRight1_TapLeft1;
    int samplesDelayRight1_TapLeft2;
    int samplesDecayDiffusion2R_TapLeft;
    int samplesDelayRight2_TapLeft;
    int samplesDelayLeft1_TapLeft;
    int samplesDecayDiffusion2L_TapLeft;
    int samplesDelayLeft2_TapLeft;

    int samplesDelayLeft1_TapRight1;
    int samplesDelayLeft1_TapRight2;
    int samplesDecayDiffusion2L_TapRight;
    int samplesDelayLeft2_TapRight;
    int samplesDelayRight1_TapRight;
    int samplesDecayDiffusion2R_TapRight;
    int samplesDelayRight2_TapRight;
};

/*
  ==============================================================================

    DelayLine.h
    Created: 17 May 2024 11:15:30am
    Author:  Till

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DelayLine
{
public:
    DelayLine() = default;

    void prepareToPlay (int maxDelaySamples);

    void pushSample (float sample);

    float getSample (int delayInSamples);

private:
    Array<float> buffer;
    int writeIndex{ 0 };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayLine)
};


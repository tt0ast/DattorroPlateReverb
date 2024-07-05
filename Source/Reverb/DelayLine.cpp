/*
  ==============================================================================

    DelayLine.cpp
    Created: 17 May 2024 11:15:30am
    Author:  Till

  ==============================================================================
*/

// "C:\Program Files\REAPER (x64)reaper.exe"

#include "DelayLine.h"

void DelayLine::prepareToPlay (int maxDelayInSamples)
{
    buffer.resize (maxDelayInSamples);
    buffer.fill (0.f);
}

void DelayLine::pushSample (float sample)
{
    buffer.getReference (writeIndex) = sample;
    writeIndex++;
    if (writeIndex >= buffer.size()) {
        writeIndex -= buffer.size();
    }
    //writeIndex = ++writeIndex % buffer.size();
}

float DelayLine::getSample(int delayInSamples)
{
    if (delayInSamples < buffer.size())
    {
        auto offset = writeIndex - delayInSamples;

        if (offset < 0)
            offset += buffer.size();
            
        return buffer.getReference (offset);

    }

    jassertfalse;
    return 0.f;
}
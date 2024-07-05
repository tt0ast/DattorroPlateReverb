/*
  ==============================================================================

    PlateReverb.cpp
    Created: 17 May 2024 12:00:32pm
    Author:  Till

  ==============================================================================
*/

#include "PlateReverb.h"

PlateReverb::PlateReverb()
{
    // set base parameters
    predelayTime = 0.0;
    decay = 0.5;
    decayDiffusion1 = 0.7;
    decayDiffusion2 = 0.5;
    inputDiffusion1 = 0.75;
    inputDiffusion2 = 0.625;
    bandwidth = 0.9995;
    damping = 0.0005;
    mix = 0.5;

    // write max delay times in milliseconds to the hash map
    delayTimesMilsec.set("predelay", 1000);
    delayTimesMilsec.set("predelayTap", predelayTime);

    delayTimesMilsec.set ("delayLeft1", 149.62534);
    delayTimesMilsec.set ("delayLeft2", 124.99579);
    delayTimesMilsec.set ("delayRight1", 141.69550);
    delayTimesMilsec.set ("delayRight2", 106.28003);

    delayTimesMilsec.set ("inputDiffusion1A", 4.77134);
    delayTimesMilsec.set ("inputDiffusion1B", 3.59530);
    delayTimesMilsec.set ("inputDiffusion2A", 12.7348);
    delayTimesMilsec.set ("inputDiffusion2B", 9.30748);

    delayTimesMilsec.set ("decayDiffusion1L", 22.57988);
    delayTimesMilsec.set ("decayDiffusion2L", 60.48183);
    delayTimesMilsec.set ("decayDiffusion1R", 30.50972);
    delayTimesMilsec.set ("decayDiffusion2R", 89.24431);

    // write output tap values in milliseconds to the hash map
    delayTimesMilsec.set ("delayRight1_TapLeft1", 8.93787);
    delayTimesMilsec.set ("delayRight1_TapLeft2", 99.92943);
    delayTimesMilsec.set ("decayDiffusion2R_TapLeft", 64.27875);
    delayTimesMilsec.set ("delayRight2_TapLeft", 67.06763);
    delayTimesMilsec.set ("delayLeft1_TapLeft", 66.86603);
    delayTimesMilsec.set ("decayDiffusion2L_TapLeft", 6.28339);
    delayTimesMilsec.set ("delayLeft2_TapLeft", 35.81868);

    delayTimesMilsec.set ("delayLeft1_TapRight1", 11.86116);
    delayTimesMilsec.set ("delayLeft1_TapRight2", 121.87090);
    delayTimesMilsec.set ("decayDiffusion2L_TapRight", 41.26205);
    delayTimesMilsec.set ("delayLeft2_TapRight", 89.81553);
    delayTimesMilsec.set ("delayRight1_TapRight", 70.93175);
    delayTimesMilsec.set ("decayDiffusion2R_TapRight", 11.25634);
    delayTimesMilsec.set ("delayRight2_TapRight", 4.06572);

    // initialise samples values
    samplesPredelayTap = 0;

    samplesDelayRight1_TapLeft1 = 0;
    samplesDelayRight1_TapLeft2 = 0;
    samplesDecayDiffusion2R_TapLeft = 0;
    samplesDelayRight2_TapLeft = 0;
    samplesDelayLeft1_TapLeft = 0;
    samplesDecayDiffusion2L_TapLeft = 0;
    samplesDelayLeft2_TapLeft = 0;

    samplesDelayLeft1_TapRight1 = 0;
    samplesDelayLeft1_TapRight2 = 0;
    samplesDecayDiffusion2L_TapRight = 0;
    samplesDelayLeft2_TapRight = 0;
    samplesDelayRight1_TapRight = 0;
    samplesDecayDiffusion2R_TapRight = 0;
    samplesDelayRight2_TapRight = 0;
}

void PlateReverb::prepareToPlay(double sampleRate)
{
    // convert all delay times to samples using new sample rate
    delayTimesSamp.clear();

    for (HashMap<String, float>::Iterator i(delayTimesMilsec); i.next();)
    {
        delayTimesSamp.set(i.getKey(), int(i.getValue() * (sampleRate / 1000)));
    }

    // set tap time in samples for output and predelay
    samplesPredelayTap = delayTimesSamp["predelayTap"];

    samplesDelayRight1_TapLeft1 = delayTimesSamp["delayRight1_TapLeft1"];
    samplesDelayRight1_TapLeft2 = delayTimesSamp["delayRight1_TapLeft2"];
    samplesDecayDiffusion2R_TapLeft = delayTimesSamp["decayDiffusion2R_TapLeft"];
    samplesDelayRight2_TapLeft = delayTimesSamp["delayRight2_TapLeft"];
    samplesDelayLeft1_TapLeft = delayTimesSamp["delayLeft1_TapLeft"];
    samplesDecayDiffusion2L_TapLeft = delayTimesSamp["decayDiffusion2L_TapLeft"];
    samplesDelayLeft2_TapLeft = delayTimesSamp["delayLeft2_TapLeft"];

    samplesDelayLeft1_TapRight1 = delayTimesSamp["delayLeft1_TapRight1"];
    samplesDelayLeft1_TapRight2 = delayTimesSamp["delayLeft1_TapRight2"];
    samplesDecayDiffusion2L_TapRight = delayTimesSamp["decayDiffusion2L_TapRight"];
    samplesDelayLeft2_TapRight = delayTimesSamp["delayLeft2_TapRight"];
    samplesDelayRight1_TapRight = delayTimesSamp["delayRight1_TapRight"];
    samplesDecayDiffusion2R_TapRight = delayTimesSamp["decayDiffusion2R_TapRight"];
    samplesDelayRight2_TapRight = delayTimesSamp["delayRight2_TapRight"];

    // initialize delay lines with correct max samples
    predelay.prepareToPlay(delayTimesSamp["predelay"]);

    delayLeft1.prepareToPlay(delayTimesSamp["delayLeft1"]);
    delayLeft2.prepareToPlay(delayTimesSamp["delayLeft2"]);
    delayRight1.prepareToPlay(delayTimesSamp["delayRight1"]);
    delayRight2.prepareToPlay(delayTimesSamp["delayRight2"]);

    inputDiffusion1A.prepareToPlay(delayTimesSamp["inputDiffusion1A"]);
    inputDiffusion1B.prepareToPlay(delayTimesSamp["inputDiffusion1B"]);
    inputDiffusion2A.prepareToPlay(delayTimesSamp["inputDiffusion2A"]);
    inputDiffusion2B.prepareToPlay(delayTimesSamp["inputDiffusion2B"]);

    decayDiffusion1L.prepareToPlay(delayTimesSamp["decayDiffusion1L"]);
    decayDiffusion2L.prepareToPlay(delayTimesSamp["decayDiffusion2L"]);
    decayDiffusion1R.prepareToPlay(delayTimesSamp["decayDiffusion1R"]);
    decayDiffusion2R.prepareToPlay(delayTimesSamp["decayDiffusion2R"]);

    bandwidthOnepole.prepareToPlay(2);
    dampingOnepoleLeft.prepareToPlay(2);
    dampingOnepoleRight.prepareToPlay(2);

}

void PlateReverb::processBlock(juce::AudioBuffer<float>& buffer, int numSamples, int numChannels)
{
    float* writeBufferL = buffer.getWritePointer (0);
    float* writeBufferR = buffer.getWritePointer (1);
    auto* readBufferL = buffer.getReadPointer (0);
    auto* readBufferR = buffer.getReadPointer (1);

    for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++)
    {
        // get input signal and sum left + right channels
        float sample = (readBufferL[sampleIndex] + readBufferR[sampleIndex]) * float(0.5);

        // predelay
        if (predelayTime != 0)
        {
            float samplePredelay = predelay.getSample (samplesPredelayTap);
            predelay.pushSample (sample);
            sample = samplePredelay;
        }
        else
        {
            predelay.pushSample (sample);
        }

        // input signal bandwidth control
        sample = sample * bandwidth;
        sample = calculateOnepole (sample, float(1.0) - bandwidth, bandwidthOnepole);

        // input diffusion
        sample = calculateLattice (sample, inputDiffusion1, inputDiffusion1A);
        sample = calculateLattice (sample, inputDiffusion1, inputDiffusion1B);
        sample = calculateLattice (sample, inputDiffusion2, inputDiffusion2A);
        sample = calculateLattice (sample, inputDiffusion2, inputDiffusion2B);


        // reverb tank
        float reverbTankInput = sample;

        // reverb tank left  
        sample = sample + (delayRight2.getSample(0) * decay);
        sample = calculateReverseLattice (sample, decayDiffusion1, decayDiffusion1L);

        sample = processDelay (sample, delayLeft1);

        sample = sample * (float(1.0) - damping);
        sample = calculateOnepole (sample, damping, dampingOnepoleLeft);

        sample = sample * decay;
        sample = calculateLattice (sample, decayDiffusion2, decayDiffusion2L);

        sample = processDelay (sample, delayLeft2);
        sample = sample * decay;

        // reverb tank right
        sample = sample + reverbTankInput;
        sample = calculateReverseLattice (sample, decayDiffusion1, decayDiffusion1R);

        sample = processDelay (sample, delayRight1);

        sample = sample * (float(1.0) - damping);
        sample = calculateOnepole (sample, damping, dampingOnepoleRight);

        sample = sample * decay;
        sample = calculateLattice (sample, decayDiffusion2, decayDiffusion2R);

        processDelay (sample, delayRight2);

        // output
        float outputLeft;
        outputLeft = float(0.6) * delayRight1.getSample (samplesDelayRight1_TapLeft1 + 1);
        outputLeft += float(0.6) * delayRight1.getSample (samplesDelayRight1_TapLeft2 + 1);
        outputLeft -= float(0.6) * decayDiffusion2R.getSample (samplesDecayDiffusion2R_TapLeft + 1);
        outputLeft += float(0.6) * delayRight2.getSample (samplesDelayRight2_TapLeft + 1);
        outputLeft -= float(0.6) * delayLeft1.getSample (samplesDelayLeft1_TapLeft + 1);
        outputLeft -= float(0.6) * decayDiffusion2L.getSample (samplesDecayDiffusion2L_TapLeft + 1);
        outputLeft -= float(0.6) * delayLeft2.getSample (samplesDelayLeft2_TapLeft + 1);

        float outputRight;
        outputRight = float(0.6) * delayLeft1.getSample (samplesDelayLeft1_TapRight1 + 1);
        outputRight += float(0.6) * delayLeft1.getSample (samplesDelayLeft1_TapRight2 + 1);
        outputRight -= float(0.6) * decayDiffusion2L.getSample (samplesDecayDiffusion2L_TapRight + 1);
        outputRight += float(0.6) * delayLeft2.getSample (samplesDelayLeft2_TapRight + 1);
        outputRight -= float(0.6) * delayRight1.getSample (samplesDelayRight1_TapRight + 1);
        outputRight -= float(0.6) * decayDiffusion2R.getSample (samplesDecayDiffusion2R_TapRight + 1);
        outputRight -= float(0.6) * delayRight2.getSample (samplesDelayRight2_TapRight + 1);

        writeBufferL[sampleIndex] = (readBufferL[sampleIndex] * (1.0 - mix)) + (outputLeft * mix);
        writeBufferR[sampleIndex] = (readBufferR[sampleIndex] * (1.0 - mix)) + (outputRight * mix);
    }

}

float PlateReverb::calculateLattice (float sample, float coefficient, DelayLine& delayLine)
{
    float delayOutput = delayLine.getSample (0);
    float delayInput = sample - (delayOutput * coefficient);
    float output = delayInput * coefficient + delayOutput;
    delayLine.pushSample (delayInput);

    return output;
}

float PlateReverb::calculateReverseLattice (float sample, float coefficient, DelayLine& delayLine)
{
    float delayOutput = delayLine.getSample (0);
    float delayInput = sample + (delayOutput * coefficient);
    float output = delayOutput - (delayInput * coefficient);
    delayLine.pushSample (delayInput);

    return output;
}

float PlateReverb::calculateOnepole (float sample, float coefficient, DelayLine& delayLine)
{
    float output = delayLine.getSample (0) * coefficient + sample;
    delayLine.pushSample (output);

    return output;
}

float PlateReverb::processDelay (float sample, DelayLine& delayLine)
{
    float delayOutput = delayLine.getSample (0);
    delayLine.pushSample (sample);
    
    return delayOutput;
}

int PlateReverb::clamp (int low, int high, int value)
{
    if (value < low)
        return low;
    else if (value > high)
        return high;
    else
        return value;
}

float PlateReverb::clamp (float low, float high, float value)
{
    if (value < low)
        return low;
    else if (value > high)
        return high;
    else
        return value;
}

void PlateReverb::setPredelayTime (int newPredelayTime)
{
    predelayTime = clamp (0, 1000, newPredelayTime);
}

void PlateReverb::setDecay (float newDecay)
{
    decay = clamp (0.01f, 0.99f, newDecay);
    decayDiffusion2 = clamp (0.25f, 0.5f, decay + 0.15);
}

void PlateReverb::setDecayDiffusion1 (float newDecayDiffusion1)
{
    decayDiffusion1 = clamp (0.01f, 0.99f, newDecayDiffusion1);
}

void PlateReverb::setInputDiffusion1 (float newInputDiffusion1)
{
    inputDiffusion1 = clamp(0.01f, 0.99f, newInputDiffusion1);
}

void PlateReverb::setInputDiffusion2 (float newInputDiffusion2)
{
    inputDiffusion2 = clamp(0.01f, 0.99f, newInputDiffusion2);
}

void PlateReverb::setBandwidth (float newBandwidth)
{
    bandwidth = clamp (0.0000001f, 0.9999999f, newBandwidth);
}

void PlateReverb::setDamping (float newDamping)
{
    damping = clamp (0.0f, 0.9999999f, newDamping);
}

void PlateReverb::setMix(float newMix)
{
    mix = clamp (0.0f, 1.0f, newMix);
}
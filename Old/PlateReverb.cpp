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
    predelayTime = 0;
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
}

void PlateReverb::prepareToPlay (int sampleRate)
{
    // convert all delay times to samples using new sample rate
    delayTimesSamp.clear();

    for (HashMap<String, float>::Iterator i(delayTimesMilsec); i.next();)
    {
        delayTimesSamp.set(i.getKey(), int(i.getValue() * sampleRate / 1000));
    }
       
    // initialize delay lines with correct max samples
    predelay.prepareToPlay (delayTimesSamp["predelay"]);
    
    delayLeft1.prepareToPlay (delayTimesSamp["delayLeft1"]);
    delayLeft2.prepareToPlay (delayTimesSamp["delayLeft2"]);
    delayRight1.prepareToPlay (delayTimesSamp["delayRight1"]);
    delayRight2.prepareToPlay (delayTimesSamp["delayRight2"]);

    inputDiffusion1A.prepareToPlay (delayTimesSamp["inputDiffusion1A"]);
    inputDiffusion1B.prepareToPlay (delayTimesSamp["inputDiffusion1B"]);
    inputDiffusion2A.prepareToPlay (delayTimesSamp["inputDiffusion2A"]);
    inputDiffusion2B.prepareToPlay (delayTimesSamp["inputDiffusion2B"]);

    decayDiffusion1L.prepareToPlay (delayTimesSamp["decayDiffusion1L"]);
    decayDiffusion2L.prepareToPlay (delayTimesSamp["decayDiffusion2L"]);
    decayDiffusion1R.prepareToPlay (delayTimesSamp["decayDiffusion1R"]);
    decayDiffusion2R.prepareToPlay (delayTimesSamp["decayDiffusion2R"]);

    bandwidthOnepole.prepareToPlay (2);
    dampingOnepoleLeft.prepareToPlay (2);
    dampingOnepoleRight.prepareToPlay (2);
}

void PlateReverb::processBlock(juce::AudioBuffer<float>& buffer, int numSamples, int numChannels)
{
    float* writeBufferL = buffer.getWritePointer (0);
    float* writeBufferR = buffer.getWritePointer (1);
    auto* readBufferL = buffer.getReadPointer (0);
    auto* readBufferR = buffer.getReadPointer (1);

    for (int sampleIndex = 0; sampleIndex <= numSamples; sampleIndex++)
    {
        // get input signal and sum left + right channels
        float sample = (readBufferL[sampleIndex] + readBufferR[sampleIndex]) * 0.5;

        // predelay
        if (predelayTime != 0)
        {
            float samplePredelay = predelay.getSample (delayTimesSamp["predelayTap"]);
            predelay.pushSample (sample);
            sample = samplePredelay;
        }
        else
        {
            predelay.pushSample (sample);
        }

        // input signal bandwidth control
        sample = sample * bandwidth;
        sample = calculateOnepole (sample, 1 - bandwidth, bandwidthOnepole);

        // input diffusion
        sample = calculateLattice (sample, inputDiffusion1, inputDiffusion1A);
        sample = calculateLattice (sample, inputDiffusion1, inputDiffusion1B);
        sample = calculateLattice (sample, inputDiffusion2, inputDiffusion2A);
        sample = calculateLattice (sample, inputDiffusion2, inputDiffusion2B);

        // reverb tank
        float reverbTankInput = sample;

        // reverb tank left  
        sample = sample + (delayRight2.getSample(1) * decay);
        sample = calculateReverseLattice (sample, decayDiffusion1, decayDiffusion1L);

        sample = processDelay (sample, delayLeft1);

        sample = sample * (1.0 - damping);
        sample = calculateOnepole (sample, damping, dampingOnepoleLeft);

        sample = sample * decay;
        sample = calculateLattice (sample, decayDiffusion2, decayDiffusion2L);

        sample = processDelay (sample, delayLeft2);
        sample = sample * decay;

        // reverb tank right
        sample = sample + reverbTankInput;
        sample = calculateReverseLattice (sample, decayDiffusion1, decayDiffusion1R);

        sample = processDelay (sample, delayRight1);

        sample = sample * (1.0 - damping);
        sample = calculateOnepole (sample, damping, dampingOnepoleRight);

        sample = sample * decay;
        sample = calculateLattice (sample, decayDiffusion2, decayDiffusion2R);

        sample = processDelay (sample, delayRight2);

        // output
        float outputLeft;
        outputLeft = 0.6 * delayRight1.getSample (delayTimesSamp["delayRight1_TapLeft1"] - 1);
        outputLeft += 0.6 * delayRight1.getSample (delayTimesSamp["delayRight1_TapLeft2"] - 1);
        outputLeft -= 0.6 * decayDiffusion2R.getSample (delayTimesSamp["decayDiffusion2R_TapLeft"] - 1);
        outputLeft += 0.6 * delayRight2.getSample (delayTimesSamp["delayRight2_TapLeft"] - 1);
        outputLeft -= 0.6 * delayLeft1.getSample (delayTimesSamp["delayLeft1_TapLeft"] - 1);
        outputLeft -= 0.6 * decayDiffusion2L.getSample (delayTimesSamp["decayDiffusion2L_TapLeft"] - 1);
        outputLeft -= 0.6 * delayLeft2.getSample (delayTimesSamp["delayLeft2_TapLeft"] - 1);

        float outputRight;
        outputRight = 0.6 * delayLeft1.getSample(delayTimesSamp["delayLeft1_TapRight1"] - 1);
        outputRight += 0.6 * delayLeft1.getSample(delayTimesSamp["delayLeft1_TapRight2"] - 1);
        outputRight -= 0.6 * decayDiffusion2L.getSample(delayTimesSamp["decayDiffusion2L_TapRight"] - 1);
        outputRight += 0.6 * delayLeft2.getSample(delayTimesSamp["delayLeft2_TapRight"] - 1);
        outputRight -= 0.6 * delayRight1.getSample(delayTimesSamp["delayRight1_TapRight"] - 1);
        outputRight -= 0.6 * decayDiffusion2R.getSample(delayTimesSamp["decayDiffusion2R_TapRight"] - 1);
        outputRight -= 0.6 * delayRight2.getSample(delayTimesSamp["delayRight2_TapRight"] - 1);

        writeBufferL[sampleIndex] = readBufferL[sampleIndex] * (1 - mix) + outputLeft * mix;
        writeBufferR[sampleIndex] = readBufferR[sampleIndex] * (1 - mix) + outputRight * mix;
    }
}

float PlateReverb::calculateLattice (float sample, float coefficient, DelayLine& delayLine)
{
    float delayOutput = delayLine.getSample (1);
    float delayInput = sample - (delayOutput * coefficient);
    float output = delayInput * coefficient + delayOutput;
    delayLine.pushSample (delayInput);

    return output;
}

float PlateReverb::calculateReverseLattice (float sample, float coefficient, DelayLine& delayLine)
{
    float delayOutput = delayLine.getSample (1);
    float delayInput = sample + (delayOutput * coefficient);
    float output = delayOutput - (delayInput * coefficient);
    delayLine.pushSample (delayInput);

    return output;
}

float PlateReverb::calculateOnepole (float sample, float coefficient, DelayLine& delayLine)
{
    float output = delayLine.getSample (1) * coefficient + sample;
    delayLine.pushSample (output);

    return output;
}

float PlateReverb::processDelay (float sample, DelayLine& delayLine)
{
    float delayOutput = delayLine.getSample (1);
    delayLine.pushSample (sample);
    
    return delayOutput;
}


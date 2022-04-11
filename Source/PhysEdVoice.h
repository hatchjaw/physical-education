//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef PHYSICAL_EDUCATION_PHYSEDVOICE_H
#define PHYSICAL_EDUCATION_PHYSEDVOICE_H

#include <JuceHeader.h>
#include "Resonators/Resonator.h"

class PhysEdVoice : public juce::SynthesiserVoice {
public:
    ~PhysEdVoice() override;

    bool canPlaySound(SynthesiserSound *sound) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock, int numOutputChannels);

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound,
                   int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(
            AudioBuffer<float> &outputBuffer,
            int startSample,
            int numSamples
    ) override;

    void setResonator(Resonator *resonatorToUse);

    /**
     *
     * @return A reference to the resonator's current state vector.
     */
    std::vector<double> &getResonatorState();

private:
    bool isPrepared;
    Resonator *resonator;
    juce::AudioBuffer<float> buffer;
};


#endif //PHYSICAL_EDUCATION_PHYSEDVOICE_H

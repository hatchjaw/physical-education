//
// Created by Tommy Rushton on 23/03/2022.
//

#include "PhysEdVoice.h"
#include "PhysEdSound.h"
#include "Resonators/StiffString.h"
#include "Utils.h"

PhysEdVoice::~PhysEdVoice() {
}

bool PhysEdVoice::canPlaySound(SynthesiserSound *sound) {
    return dynamic_cast<PhysEdSound *>(sound) != nullptr;
}

void PhysEdVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numOutputChannels) {
    this->resonator->setDecayTimes(15, 10000);
    if (auto model = dynamic_cast<StiffString *>(this->resonator)) {
        model->setWavespeed(490.f);
        model->setStiffness(2.f);
    }
    this->resonator->initialiseModel(static_cast<float>(sampleRate));
    this->resonator->setOutputPosition(.9f);
    this->isPrepared = true;
}

void PhysEdVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound,
                            int currentPitchWheelPosition) {
    this->resonator->excite(.5, 10, .1f * velocity);
}

void PhysEdVoice::stopNote(float velocity, bool allowTailOff) {

}

void PhysEdVoice::pitchWheelMoved(int newPitchWheelValue) {

}

void PhysEdVoice::controllerMoved(int controllerNumber, int newControllerValue) {

}

void PhysEdVoice::renderNextBlock(
        AudioBuffer<float> &outputBuffer,
        int startSample,
        int numSamples
) {
    jassert(this->isPrepared);

    if (!this->isVoiceActive()) {
        return;
    }

    // MIDI messages can occur at any point during a buffer,
    // so prevent discontinuities by writing to a temp buffer.
    this->buffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    this->buffer.clear();

    auto originalNumSamples = numSamples;
    auto originalStartSample = startSample;
    startSample = 0;

    while (--numSamples >= 0) {
        this->resonator->updateState();
        auto currentSample = Utils::clamp(this->resonator->getOutput(), -1.f, 1.f);

        for (auto i = (int) this->buffer.getNumChannels(); --i >= 0;) {
            this->buffer.addSample((int) i, startSample, currentSample);
        }

        ++startSample;
    }

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
        outputBuffer.addFrom(channel, originalStartSample, this->buffer, channel, 0,
                             originalNumSamples);
    }
}

void PhysEdVoice::setResonator(Resonator *resonatorToUse) {
    this->resonator = resonatorToUse;
}

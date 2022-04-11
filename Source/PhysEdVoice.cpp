//
// Created by Tommy Rushton on 23/03/2022.
//

#include "PhysEdVoice.h"
#include "PhysEdSound.h"
#include "Resonators/StiffString.h"
#include "Utils.h"
#include "Exciters/Bow.h"

PhysEdVoice::~PhysEdVoice() = default;

bool PhysEdVoice::canPlaySound(SynthesiserSound *sound) {
    return dynamic_cast<PhysEdSound *>(sound) != nullptr;
}

void PhysEdVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numOutputChannels) {
    this->resonator->setDecayTimes(13.5, 3000);
    if (auto model = dynamic_cast<StiffString *>(this->resonator)) {
        model->setDensity(7850.);
        model->setRadius(.0005);
        model->setTension(100.);
        model->setYoungsModulus(2e11);
        if (auto exciter = dynamic_cast<Bow *>(model->getExciter())) {
            exciter->setFriction(100.);
        }
    }
    this->resonator->initialiseModel(static_cast<float>(sampleRate));
    this->resonator->setOutputPosition(.9f);
    this->resonator->setOutputPositions(std::pair<float, float>{.35, .9});
    this->isPrepared = true;
}

void PhysEdVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound,
                            int currentPitchWheelPosition) {
    this->resonator->excite(static_cast<float>(midiNoteNumber) / 127.f, velocity, velocity);
}

void PhysEdVoice::stopNote(float velocity, bool allowTailOff) {
    this->resonator->damp();
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
//        auto currentSample = Utils::clamp(this->resonator->getOutput(), -1.f, 1.f);
        auto currentSamples = this->resonator->getOutputStereo();
        auto samp1 = Utils::clamp(currentSamples.first, -1.0f, 1.0f);
        auto samp2 = Utils::clamp(currentSamples.second, -1.0f, 1.0f);

        for (auto i = (int) this->buffer.getNumChannels(); --i >= 0;) {
            auto sample = i % 2 == 0 ? samp2 : samp1;
            this->buffer.addSample((int) i, startSample, sample);
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


std::vector<double> &PhysEdVoice::getResonatorState() {
    return this->resonator->getState();
}

//
// Created by Tommy Rushton on 23/03/2022.
//

#include "PhysEdVoice.h"
#include "PhysEdSound.h"
#include "Resonators/StiffString.h"
#include "Utils.h"
#include "Exciters/Bow.h"
#include "Resonators/Dynamic1dWave.h"

PhysEdVoice::~PhysEdVoice() = default;

bool PhysEdVoice::canPlaySound(SynthesiserSound *sound) {
    return dynamic_cast<PhysEdSound *>(sound) != nullptr;
}

void PhysEdVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numOutputChannels) {
    resonator->setDecayTimes(12.5, 1.3323);

    if (auto model = dynamic_cast<StiffString *>(resonator)) {
        model->setDensity(7850.);
        model->setRadius(.0005);
        model->setTension(100.);
        model->setYoungsModulus(2e11);
        if (auto exciter = dynamic_cast<Bow *>(model->getExciter())) {
            exciter->setFriction(100.);
        }
    } else if (auto model = dynamic_cast<Dynamic1dWave *>(resonator)) {
        model->setDecayTimes(18.5, 18);
        model->setLength(1.05);
        model->setDensity(1000.);
        model->setRadius(9e-4);
        model->setTension(1000.);
        if (auto exciter = dynamic_cast<Bow *>(model->getExciter())) {
            exciter->setFriction(100.);
        }
    }

    this->resonator->initialiseModel(static_cast<float>(sampleRate));
    this->resonator->setOutputPositions(std::vector<float>{.35f, .9f});
    this->resonator->setOutputMode(Resonator::OutputMode::VELOCITY);
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

//    if (!this->isVoiceActive()) {
//        while (--numSamples >= 0) {
//            resonator->updateSmoothedParams();
//        }
//        return;
//    }

    // MIDI messages can occur at any point during a buffer,
    // so prevent discontinuities by writing to a temp buffer.
    auto numChannels = outputBuffer.getNumChannels();
    this->buffer.setSize(numChannels, numSamples, false, false, true);
    this->buffer.clear();

    auto originalNumSamples = numSamples;
    auto originalStartSample = startSample;
    startSample = 0;

    while (--numSamples >= 0) {
        resonator->updateState();

        auto samples = resonator->getOutput(numChannels);

        for (auto i = (int) this->buffer.getNumChannels(); --i >= 0;) {
            auto sample = Utils::clamp(samples[i], -1.0f, 1.0f);
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


std::vector<FType> &PhysEdVoice::getResonatorState() {
    return this->resonator->getState();
}

Resonator *PhysEdVoice::getResonator() {
    return resonator;
}

Resonator *&PhysEdVoice::getResonatorPtr() {
    return resonator;
}

//
// Created by Tommy Rushton on 23/03/2022.
//

#include <cmath>
#include <utility>
#include "Resonator.h"

Resonator::Resonator(std::pair<unsigned int, unsigned int> stencil, Exciter *exciterToUse) :
        stencilDimensions(std::move(stencil)),
        exciter(exciterToUse) {
    u.resize(stencilDimensions.second);
}

void Resonator::setDecayTimes(FType freqIndependent, FType freqDependent) {
    parameters.T60_0 = freqIndependent;
    parameters.derived.sigma0 = t60ToSigma(parameters.T60_0);
    parameters.T60_1 = freqDependent;
    parameters.derived.sigma1 = t60ToSigma(parameters.T60_1);
}

void Resonator::setOutputPosition(float outputPosition) {
    jassert(parameters.derived.N > 0);
    outputPosition = Utils::clamp(outputPosition, 0.f, 1.f);
    outputIndex = static_cast<int>(round(static_cast<float>(parameters.derived.N) *
                                         outputPosition));
}

void Resonator::setExciter(Exciter *exciterToUse) {
    this->exciter = exciterToUse;
}

void Resonator::initialiseModel(FType sampleRate) {
    parameters.derived.k = 1.0 / sampleRate;
    parameters.derived.kSq = pow(parameters.derived.k, 2);
    computeCoefficients();
    exciter->setupExcitation();
    initialiseState();
    isInitialised = true;
}

void Resonator::excite(float position, float force, float velocity) {
    jassert(isInitialised);

    exciter->startExcitation(position, force, velocity);
}

void Resonator::damp() {
    exciter->stopExcitation();
}

FType Resonator::getOutput() {
    jassert(isInitialised);
    return getOutputScalar() * u[1][outputIndex];
}

FType Resonator::t60ToSigma(FType t60) {
    return (6 * logf(10)) / t60;
}

void Resonator::initialiseState() {
    // Resize and initialise uStates.
    uStates.resize(stencilDimensions.second, std::vector<FType>(parameters.derived.N + 1, 0.0));
    // Point each element in u to the address of the start of the corresponding
    // vector in uStates.
    for (unsigned long i = 0; i < stencilDimensions.second; ++i) {
        u[i] = &uStates[i][0];
    }
}

std::vector<FType> &Resonator::getState() {
    jassert(isInitialised);
    return uStates[1];
}

ModelParameters &Resonator::getParameters() {
    return parameters;
}

Exciter *Resonator::getExciter() {
    return exciter;
}

void Resonator::advanceTimestep() {
    jassert(isInitialised);
    // Swap pointers to advance the time-step.
    auto uTemp = u[0];
    u[0] = u[1];
    u[1] = u[2];
    u[2] = uTemp;
}

void Resonator::updateState() {
    jassert(isInitialised);
    computeScheme();
    if (exciter->isExciting) {
        exciter->applyExcitation(u);
    }
    advanceTimestep();
}

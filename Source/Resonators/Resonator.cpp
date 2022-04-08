//
// Created by Tommy Rushton on 23/03/2022.
//

#include <cmath>
#include <algorithm>
#include <utility>
#include "Resonator.h"
#include "../Utils.h"

Resonator::Resonator(std::pair<unsigned int, unsigned int> stencil, Exciter *exciterToUse) :
        stencilDimensions(std::move(stencil)),
        exciter(exciterToUse) {
    u.resize(stencilDimensions.second);
}

void Resonator::setDecayTimes(FType freqIndependent, FType freqDependent) {
    T60_0 = freqIndependent;
    T60_1 = freqDependent;
}

void Resonator::setOutputPosition(float outputPosition) {
    jassert(N > 0);
    outputPosition = Utils::clamp(outputPosition, 0.f, 1.f);
    outputIndex = static_cast<int>(round(static_cast<float>(N) * outputPosition));
}

void Resonator::setExciter(Exciter *exciterToUse) {
    this->exciter = exciterToUse;
}

void Resonator::initialiseModel(FType sampleRate) {
    k = 1.0 / sampleRate;
    computeCoefficients();
    exciter->setNumGridPoints(N);
    initialiseState();
    isInitialised = true;
}

void Resonator::excite(float position, float force, float velocity) {
    jassert(isInitialised);

    exciter->initialiseExcitation(position, force, velocity);
}

FType Resonator::getOutput() {
    jassert(isInitialised);
    return u[1][outputIndex];
}

FType Resonator::t60ToSigma(FType t60) {
    return (6 * logf(10)) / t60;
}

void Resonator::initialiseState() {
    // Resize and initialise uStates.
    uStates.resize(stencilDimensions.second, std::vector<FType>(N + 1, 0.0));
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
    exciter->applyExcitation(u);
    advanceTimestep();
}

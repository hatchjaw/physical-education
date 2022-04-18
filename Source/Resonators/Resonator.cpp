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

void Resonator::setOutputPositions(std::vector<float> outputPositionsToUse) {
    jassert(parameters.derived.N > 0);

    if (outputPositionsToUse.size() != outputPositions.size()) {
        Utils::setupVectorPointers(uOut, uOutStates, 3, outputPositionsToUse.size());

        outputPositions.resize(outputPositionsToUse.size());
        normalisedOutputPositions.resize(outputPositionsToUse.size());
    }

    // TODO: prevent discontinuities when output positions change.
    for (unsigned long i = 0; i < outputPositionsToUse.size(); ++i) {
        auto position = Utils::clamp(outputPositionsToUse[i], 0.f, 1.f);
        normalisedOutputPositions[i] = position;
        position *= static_cast<float>(parameters.derived.N);
        outputPositions[i] = position;
    }
}

void Resonator::setOutputMode(Resonator::OutputMode mode) {
    outputMode = mode;
}

void Resonator::setExciter(Exciter *exciterToUse) {
    exciter = exciterToUse;
    exciter->setupExcitation();
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

FType Resonator::getOutputAtPosition(unsigned long outputPositionIndex) {
    jassert(isInitialised);
    jassert(outputPositionIndex < outputPositions.size());

    // Separate the integer and fractional parts of the read position.
    float readPos;
    auto alpha = modf(outputPositions[outputPositionIndex], &readPos);
    // Adjust amplitude wrt the output position. Positions close to the
    // centre have greater displacement than positions at the extremities.
    // TODO: maybe get rid of this, as it probably only holds for strings.
    auto positionAdjustment = 2 * fabs(normalisedOutputPositions[outputPositionIndex] - .5);

    // Get the displacement, interpolated around the read position.
    auto displacement = Utils::interpolate(u[1], static_cast<int>(readPos), alpha);

    // Save it to the displacement buffer.
    uOut[0][outputPositionIndex] = displacement;

    switch (outputMode) {
        case DISPLACEMENT:
            // Just return the appropriately scaled displacement.
            return getOutputScalar() * sqrt(positionAdjustment) * displacement;
        case VELOCITY:
            // Return the appropriately scaled velocity, taken as the
            // difference between the current displacement and the displacement
            // two samples ago.
            return getOutputScalar() *
                   sqrt(positionAdjustment) *
                   (1 / (2 * parameters.derived.k)) *
                   (displacement - uOut[2][outputPositionIndex]);
        default:
            jassertfalse;
    }
}

std::vector<FType> Resonator::getOutput(unsigned long numOutputPositions) {
    jassert(isInitialised);

    auto out = std::vector<FType>(numOutputPositions);

    for (unsigned long i = 0; i < numOutputPositions; ++i) {
        out[i] = getOutputAtPosition(i);
    }

    return out;
}

FType Resonator::t60ToSigma(FType t60) {
    return (6 * logf(10)) / t60;
}

void Resonator::initialiseState() {
    Utils::setupVectorPointers(u, uStates, stencilDimensions.second, parameters.derived.N + 1);
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
    Utils::pointerSwap(u);
    // Also rotate the displacement buffer.
    Utils::pointerSwap(uOut);
}

void Resonator::updateState() {
    jassert(isInitialised);
    computeScheme();
    if (exciter->isExciting) {
        exciter->applyExcitation(u);
    }
    advanceTimestep();
}

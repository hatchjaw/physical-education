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

void Resonator::setOutputPosition(float outputPositionToUse) {
    jassert(parameters.derived.N > 0);
    outputPositionToUse = Utils::clamp(outputPositionToUse, 0.f, 1.f);
    outputPosition = outputPositionToUse * static_cast<float>(parameters.derived.N);
}

void Resonator::setOutputPositions(std::pair<float, float> outputPositionsToUse) {
    jassert(parameters.derived.N > 0);

    outputPositionsToUse.first = Utils::clamp(outputPositionsToUse.first, 0.f, 1.f);
    outputPositionsToUse.second = Utils::clamp(outputPositionsToUse.second, 0.f, 1.f);
    normalisedOutputPositions = outputPositionsToUse;
    outputPositions.first = outputPositionsToUse.first * static_cast<float>(parameters.derived.N);
    outputPositions.second = outputPositionsToUse.second * static_cast<float>(parameters.derived.N);
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
    float readPos;
    auto alpha = modf(outputPosition, &readPos);
    return getOutputScalar() * Utils::interpolate(u[1], static_cast<int>(readPos), alpha);
}

// TODO: add option of using velocity instead of displacement for output.
std::pair<FType, FType> Resonator::getOutputStereo() {
    jassert(isInitialised);

    auto getOutput = [this](float outPos, float normalisedOutPos) -> FType {
        // Separate the integer and fractional parts of the read position.
        float readPos;
        auto alpha = modf(outPos, &readPos);
        // Adjust amplitude wrt the output position. Positions close to the
        // centre have greater displacement than positions at the extremities.
        // TODO: maybe get rid of this, as it probably only holds for strings.
        auto positionAdjustment = 2 * fabs(normalisedOutPos - .5);

        switch (outputMode) {
            case DISPLACEMENT:
                return getOutputScalar() *
                       sqrt(positionAdjustment) *
                       Utils::interpolate(u[1], static_cast<int>(readPos), alpha);
            case VELOCITY:
                // Return the scaled displacement, interpolated around the read position.
                return getOutputScalar() *
                       sqrt(positionAdjustment) *
                       (1 / (2 * parameters.derived.k)) * (
                               // By now the timestep has been advanced, so u[1] is next
                               // and u[2] is prev.
                               Utils::interpolate(u[1], static_cast<int>(readPos), alpha) -
                               // TODO: find a way to cache the previous sample for each output position.
                               Utils::interpolate(u[2], static_cast<int>(readPos), alpha)
                       );
            default:
                jassertfalse;
        }
    };

    return std::pair<FType, FType>{
            getOutput(outputPositions.first, normalisedOutputPositions.first),
            getOutput(outputPositions.second, normalisedOutputPositions.second),
    };
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

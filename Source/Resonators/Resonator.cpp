//
// Created by Tommy Rushton on 23/03/2022.
//

#include <cmath>
#include <algorithm>
#include <utility>
#include "Resonator.h"
#include "../Utils.h"

Resonator::Resonator(std::pair<unsigned int, unsigned int> stencil) :
        stencilDimensions(std::move(stencil)) {
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

void Resonator::initialiseModel(FType sampleRate) {
    k = 1.0 / sampleRate;
    computeCoefficients();
    initialiseState();
    isInitialised = true;
}

void Resonator::excite(float position, int width, float force) {
    jassert(isInitialised);

    // Keep the with sensible relative to the total number of grid-points, and
    // respect the boundary conditions.
    if (width > N - 3) {
        width = N - 3;
    }
    auto halfWidth = .5f * width;
    // No need to repeatedly calculate force/2 in the loop, so do it here.
    auto forceToUse = .5f * force;
    // Calculate the excitation position as a proportion of N.
    auto pos = Utils::clamp(position, 0.f, 1.f);
    // Find the nearest integer start index; also ensure the excitation can't
    // exceed the bounds of the grid, and respect the boundary conditions.
    auto start = std::min(
            std::max(static_cast<int>(floor(N * pos - halfWidth)), 2),
            N - width
    );

    // Apply the excitation by adding displacement to the identified range of grid-points.
    for (int l = 0; l < width; ++l) {
        u[1][l + start] += forceToUse * (1 - cos((M_PI * l) / halfWidth));
    }
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
    advanceTimestep();
}

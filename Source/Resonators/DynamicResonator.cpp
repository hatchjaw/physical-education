//
// Created by Tommy Rushton on 09/08/2022.
//

#include "DynamicResonator.h"


//DynamicResonator::DynamicResonator() : Resonator() { }

void DynamicResonator::initialiseState() {
    Utils::setupVectorPointers(u, uStates, stencilDimensions.second, Mu + 1);
    Utils::setupVectorPointers(w, wStates, stencilDimensions.second, parameters.derived.N - Mu + 1);
}

std::vector<FType> &DynamicResonator::getStaticState() {
    return wStates[0];
}

void DynamicResonator::advanceTimestep() {
    Resonator::advanceTimestep();
    Utils::pointerSwap(w);
}

void DynamicResonator::setTension(float tension) {
    parameters.T.set(tension);
}

void DynamicResonator::initialiseModel(FType sampleRate) {
    parameters.derived.k = 1.0 / sampleRate;
    parameters.derived.kSq = pow(parameters.derived.k, 2);
    computeDerivedParameters();
    computeCoefficients();

    // Spatial dimensions
    Mu = parameters.derived.N - 1;
    Mw = 1;
    NPrev = parameters.derived.N;

    exciter->setupExcitation();

    initialiseState();

    // Displacement correction stuff
    // Damping coefficient for the spring force
    auto sigmaCk = sigmaC / parameters.derived.k;
    sigmaCkp = 1. + sigmaCk;
    sigmaCkm = 1. - sigmaCk;
    restoringForce = sigmaCkm / sigmaCkp;

    isInitialised = true;
}

void DynamicResonator::updateState() {
    jassert(isInitialised);
    computeDerivedParameters();
    computeCoefficients();
    adjustGridDimensions();
    computeScheme();
    doDisplacementCorrection();
    exciter->applyExcitation(u);
    advanceTimestep();
}

// TODO: DRY this out
FType DynamicResonator::getOutputAtPosition(unsigned long outputPositionIndex) {
    jassert(isInitialised);
    jassert(outputPositionIndex < normalisedOutputPositions.size());

    auto position = Utils::clamp(normalisedOutputPositions[outputPositionIndex] * NFrac, 1.f, Mu - 2.f);

    // Separate the integer and fractional parts of the read position.
    float readPos;
    auto alph = modf(position, &readPos);

    // Get the displacement, interpolated around the read position.
    auto displacement = Utils::interpolate(u[0], static_cast<int>(readPos), alph);

    // Save it to the displacement buffer.
    uOut[0][outputPositionIndex] = displacement;

    switch (outputMode) {
        case DISPLACEMENT: {
            // Adjust amplitude wrt the output position. Positions close to the
            // centre have greater displacement than positions at the extremities.
            // TODO: maybe get rid of this, as it probably only holds for strings.
            auto positionAdjustment = 2 * fabs(normalisedOutputPositions[outputPositionIndex] - .5);
            // Just return the appropriately scaled displacement.
            return getOutputScalar() * sqrt(positionAdjustment) * displacement;
        }
        case VELOCITY:
            // Return the appropriately scaled velocity, taken as the
            // difference between the current displacement and the displacement
            // two samples ago.
            return getOutputScalar() *
                   (1 / (2 * parameters.derived.k)) *
                   (displacement - uOut[2][outputPositionIndex]);
        default:
            jassertfalse;
    }
}

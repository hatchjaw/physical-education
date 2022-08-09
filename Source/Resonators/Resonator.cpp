//
// Created by Tommy Rushton on 23/03/2022.
//

#include <cmath>
#include <utility>
#include "Resonator.h"

Resonator::Resonator(std::pair<unsigned int, unsigned int> stencil, Exciter *exciterToUse) :
        stencilDimensions(std::move(stencil)),
        exciter(exciterToUse),
        damper(parameters) {
    u.resize(stencilDimensions.second);
}

void Resonator::setDecayTimes(FType freqIndependent, FType freqDependent) {
    parameters.T60_0 = freqIndependent;
    parameters.T60_1 = freqDependent;
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
        setOutputPosition(i, outputPositionsToUse[i]);
    }
}

void Resonator::setOutputPosition(unsigned long positionIndex, FType normalisedPosition) {
    auto position = Utils::clamp(normalisedPosition, 0.f, 1.f);
    normalisedOutputPositions[positionIndex] = position;
    position *= static_cast<float>(parameters.derived.N);
    outputPositions[positionIndex] = position;
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
    damper.setupInteraction();
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

    // Get the displacement, interpolated around the read position.
    auto displacement = Utils::interpolate(u[1], static_cast<int>(readPos), alpha);

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

std::vector<FType> Resonator::getOutput(unsigned long numOutputPositions) {
    jassert(isInitialised);

    auto out = std::vector<FType>(numOutputPositions);

    for (unsigned long i = 0; i < numOutputPositions; ++i) {
        out[i] = getOutputAtPosition(i);
    }

    return out;
}

FType Resonator::t60ToSigma0(FType t60) {
    return (6. * log(10.)) / t60;
}

FType Resonator::t60ToSigma1(FType t60_0, FType t60_1, FType omega) {
//    zeta1 = (-gamma^2+sqrt(gamma^4+4*K^2*(2*pi*loss(1,1))^2))/(2*K^2);
//    zeta2 = (-gamma^2+sqrt(gamma^4+4*K^2*(2*pi*loss(2,1))^2))/(2*K^2);
//    sig0 = 6*log(10)*(-zeta2/loss(1,2)+zeta1/loss(2,2))/(zeta1-zeta2);
//    sig1 = 6*log(10)*(1/loss(1,2)-1/loss(2,2))/(zeta1-zeta2);

    auto p = parameters.derived;
    auto z1 = (-p.cSq + sqrt(pow(p.c, 4) + 4 * p.kappaSq * pow(2 * M_PI * omega, 2))) / (2 * p.kappaSq);

    if (p.kappa == 0) {
        z1 = pow(2 * M_PI * omega, 2) / (p.cSq);
    }

    if (p.c == 0) {
        z1 = 2 * M_PI * omega / p.kappa;
    }

    return (6 * log(10) / z1) * (1 / t60_1 - 1 / t60_0);
}

std::pair<FType, FType> Resonator::t60ToSigma(FType t60_0, FType t60_1, FType omega0, FType omega1) const {
    auto p = parameters.derived;
    auto cSqSq = pow(p.cSq, 2);
    auto sixLogTen = 6 * log(10);
    auto sigmas = std::pair<FType, FType>{};
    auto twoPI = 2 * M_PI;

    // If this isn't a stiff resonator, just calculate damping based on
    // wavespeed.
    if (p.kappa == 0) {
        auto zeta = pow(twoPI * omega1, 2) / (p.cSq);

        sigmas.first = sixLogTen / t60_1;
        sigmas.second = (sixLogTen * (1 / t60_1 - 1 / t60_0)) / zeta;
    } else {
        auto zeta1 = 0., zeta2 = 0.;
        // If this resonator is exclusively stiff, just use kappa.
        if (p.c == 0) {
            zeta1 = twoPI * omega0 / p.kappa;
            zeta2 = twoPI * omega1 / p.kappa;
        } else {
            // Otherwise do the full thing.
            zeta1 = (-p.cSq + sqrt(cSqSq + 4 * p.kappaSq * pow(twoPI * omega0, 2))) / (2 * p.kappaSq);
            zeta2 = (-p.cSq + sqrt(cSqSq + 4 * p.kappaSq * pow(twoPI * omega1, 2))) / (2 * p.kappaSq);
        }

        sigmas.first = sixLogTen * (zeta1 / t60_1 - zeta2 / t60_0) / (zeta1 - zeta2);
        sigmas.second = sixLogTen * (1 / t60_0 - 1 / t60_1) / (zeta1 - zeta2);
    }

    return sigmas;
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
    exciter->applyExcitation(u);
    damper.applyInteraction(u);
    advanceTimestep();
}

void Resonator::setDamperParameters(float normalisedPos, float stiffness, float nonlinearity, float damping) {
    damper.setPosition(normalisedPos);
    damper.setLinearOscillatorFrequency(stiffness);
    damper.setNonlinearOscillatorFrequency(nonlinearity);
    damper.setLoss(damping);
    damper.setupInteraction();
}

void Resonator::setDamperPosition(float normalisedPosition) {
    damper.setPosition(normalisedPosition);
}

FType Resonator::getDamperPosition() {
    return damper.position.getCurrent();
}

FType Resonator::getExcitationPosition() {
    return exciter->position.getCurrent();
}

void Resonator::updateSmoothedParams() {
    exciter->position.getNext();
    damper.position.getNext();
    damper.sigmaP.getNext();
    damper.omega0.getNext();
    damper.omega1.getNext();
}
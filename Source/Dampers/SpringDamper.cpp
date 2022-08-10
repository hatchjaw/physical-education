//
// Created by Tommy Rushton on 24/04/2022.
//

#include "SpringDamper.h"

SpringDamper::SpringDamper(ModelParameters &parameters) : resonatorParameters(parameters) {
}

void SpringDamper::setLinearOscillatorFrequency(FType frequency) {
    omega0.set(frequency * 2. * M_PI);
}

void SpringDamper::setNonlinearOscillatorFrequency(FType frequency) {
    omega1.set(frequency * 2. * M_PI);
}

void SpringDamper::setLoss(FType sigma) {
    sigmaP.set(sigma);
}

void SpringDamper::setPosition(float pos) {
    position.set(pos);
}

void SpringDamper::setupInteraction() {
    auto p = resonatorParameters.derived;

    // TODO: reinstate rho*A and adjust parameter ranges.
    //  NB, not straightforward; needs more rigorous testing.
    // On paper one would factor k^2 out of the force term and have it here,
    // but since it's a component of a and b in applyInteraction(), just make use
    // of it there.
//    forceCoefficient = 1 / (resonatorParameters.rho * p.A * p.schemeDivisor);
    forceCoefficient = 1 / (p.schemeDivisor);
}

void SpringDamper::applyInteraction(std::vector<FType *> &state) {
    auto p = resonatorParameters.derived;

    // Recalculate coefficients to keep parameters up to date.
    coeffs = {
            pow(omega0.getNext(), 2) * .5,
            pow(omega1.getNext(), 4) * .5,
            sigmaP.getNext() / p.k
    };

    // Restrict interaction position so interpolation doesn't break.
    auto floatN = static_cast<float>(p.N);
    auto posIndex = Utils::clamp(floatN * position.getNext(), 4, floatN - 4);
    auto alpha = modf(posIndex, &posIndex);
    auto interactionPos = static_cast<int>(posIndex);

    // Got to update smoothed parameters before performing this check.
    if (coeffs[0] == 0 && coeffs[1] == 0 && coeffs[2] == 0) {
        return;
    }

    // Get a stencil's worth of interpolated state at the damper position.
    std::vector<FType> eta = {
            Utils::interpolate(state[1], interactionPos - 2, alpha),
            Utils::interpolate(state[1], interactionPos - 1, alpha),
            Utils::interpolate(state[1], interactionPos, alpha),
            Utils::interpolate(state[1], interactionPos + 1, alpha),
            Utils::interpolate(state[1], interactionPos + 2, alpha),
    };
    std::vector<FType> etaPrev = {
            Utils::interpolate(state[2], interactionPos - 1, alpha),
            Utils::interpolate(state[2], interactionPos, alpha),
            Utils::interpolate(state[2], interactionPos + 1, alpha),
    };

    // Compute some force equation components.
    auto q = coeffs[0] + coeffs[1] * pow(eta[2], 2);
    auto a = p.kSq * (q + coeffs[2]);
    auto b = p.kSq * (q - coeffs[2]);

    // Compute eta^{n+1}, the next displacement at the interaction position.
    // Plug a and b into the modified update equation. It's the stiff string
    // with a added to the divisor, and b added to the coefficient for u_p^{n-1}.
    // TODO: generalise this for other FDS.
    auto etaNext = (
                           p.rawCoeffs[0] * eta[2] +
                           (p.rawCoeffs[2] - b) * etaPrev[1] +
                           p.rawCoeffs[3] * (eta[0] + eta[4]) +
                           p.rawCoeffs[4] * (eta[1] + eta[3]) +
                           p.rawCoeffs[5] * (etaPrev[0] + etaPrev[2])
                   ) / (p.schemeDivisor + a);

    // Armed with eta^{n+1}, F, the damper force, can be found.
    auto damperForce = -(a * etaNext + b * etaPrev[1]);

    // Apply the damper force.
    Utils::extrapolate(state[0], interactionPos, alpha, p.h, damperForce * forceCoefficient);
}


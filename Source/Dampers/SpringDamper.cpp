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

void SpringDamper::setupCollision() {
    auto p = resonatorParameters.derived;

    coeffs = {
            pow(omega0.getNext(), 2) * .5,
            pow(omega1.getNext(), 4) * .5,
            sigmaP.getNext() / p.k
    };

    // Get next position, so it'll be updated even if no note is happening...
    position.getNext();

    // On paper one would factor k^2 out of the force term and have it here,
    // but since it's a component of a and b in applyCollision(), just make use
    // of it there.
    collisionForceCoefficient = 1 / (resonatorParameters.rho * p.A * p.schemeDivisor);
}

void SpringDamper::applyCollision(std::vector<FType *> &state) {
    auto p = resonatorParameters.derived;

    // Got to recalculate coefficients to keep parameters up to date...
    coeffs = {
            pow(omega0.getNext(), 2) * .5,
            pow(omega1.getNext(), 4) * .5,
            sigmaP.getNext() / p.k
    };

    // Restrict collision position so interpolation doesn't break.
    auto floatN = static_cast<float>(p.N);
    auto posIndex = Utils::clamp(floatN * position.getNext(), 4, floatN - 4);
    auto alpha = modf(posIndex, &posIndex);
    auto collisionPos = static_cast<int>(posIndex);

    // Get a stencil's worth of interpolated state at the collision position.
    std::vector<FType> eta = {
            Utils::interpolate(state[1], collisionPos - 2, alpha),
            Utils::interpolate(state[1], collisionPos - 1, alpha),
            Utils::interpolate(state[1], collisionPos, alpha),
            Utils::interpolate(state[1], collisionPos + 1, alpha),
            Utils::interpolate(state[1], collisionPos + 2, alpha),
    };
    std::vector<FType> etaPrev = {
            Utils::interpolate(state[2], collisionPos - 1, alpha),
            Utils::interpolate(state[2], collisionPos, alpha),
            Utils::interpolate(state[2], collisionPos + 1, alpha),
    };

    // Compute some force equation components.
    auto q = coeffs[0] + coeffs[1] * pow(eta[2], 2);
    auto a = p.kSq * (q + coeffs[2]);
    auto b = p.kSq * (q - coeffs[2]);

    // Compute eta^{n+1}, the next displacement at the collision position.
    // Plug a and b into the modified update equation. It's the stiff string
    // with a added to the divisor, and b added to the coefficient for u_p^{n-1}.
    auto etaNext = (
                           p.rawCoeffs[0] * eta[2] +
                           (p.rawCoeffs[2] - b) * etaPrev[1] +
                           p.rawCoeffs[3] * (eta[0] + eta[4]) +
                           p.rawCoeffs[4] * (eta[1] + eta[3]) +
                           p.rawCoeffs[5] * (etaPrev[0] + etaPrev[2])
                   ) / (p.schemeDivisor + a);

    // Armed with eta^{n+1}, F, the collision force, can be found.
    auto collisionForce = -(a * etaNext + b * etaPrev[1]);

    // Apply the collision force.
    Utils::extrapolate(state[0], collisionPos, alpha, p.h, collisionForce * collisionForceCoefficient);
}


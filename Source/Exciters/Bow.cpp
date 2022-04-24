//
// Created by Tommy Rushton on 08/04/2022.
//

#include <cmath>
#include "../Utils.h"
#include "Bow.h"

void Bow::setFriction(FType friction) {
    a = friction;
}

void Bow::setupExcitation() {
    auto p = resonatorParameters.derived;
    // TODO: these coefficients relate specifically to the stiff string mode so maybe move them
    //  there and pass here by reference. Or make this a StiffStringBow instead.
    coeffs = {
            2 * (1 / p.k + p.sigma0),
            2 * (p.cSq / p.hSq - 1 / p.kSq + 3 * p.kappaSq / p.hSqSq + 2 * p.sigma1 / p.k * p.hSq),
            2 * (1 / p.kSq - 2 * p.sigma1 / p.k * p.hSq),
            -(p.cSq / p.hSq + 4 * p.kappaSq / p.hSqSq + 2 * p.sigma1 / p.k * p.hSq),
            2 * p.sigma1 / p.k * p.hSq,
            p.kappaSq / p.hSqSq
    };

    phi1 = sqrt(2 * a) * exp(.5);
    nr1 = (2 / p.k) + (2 * p.sigma0);
    excitationCoefficient = (p.kSq * phi1) / (resonatorParameters.rho * p.A * p.schemeDivisor);
}

void Bow::startExcitation(float excitationPosition,
                          float excitationForce,
                          float excitationVelocity) {
    Exciter::startExcitation(excitationPosition,
                             excitationForce * FORCE_SCALAR,
                             excitationVelocity * VELOCITY_SCALAR);
}

void Bow::applyExcitation(std::vector<FType *> &state) {
    auto p = resonatorParameters.derived;

    // Restrict bow position so interpolation doesn't break.
    // The model uses bow position ± 2, and interpolation/extrapolation use
    // l-1 ≤ l ≤ l+2, so keep bow position in range 4 ≤ bp ≤ N-4
    auto floatN = static_cast<float>(p.N);
    auto bowIndex = Utils::clamp(floatN * position, 4, floatN - 4);
    auto alpha = modf(bowIndex, &bowIndex);
    auto bowPos = static_cast<int>(bowIndex);

    std::vector<FType> uB = {
            Utils::interpolate(state[1], bowPos - 2, alpha),
            Utils::interpolate(state[1], bowPos - 1, alpha),
            Utils::interpolate(state[1], bowPos, alpha),
            Utils::interpolate(state[1], bowPos + 1, alpha),
            Utils::interpolate(state[1], bowPos + 2, alpha),
    };
    std::vector<FType> uBPrev = {
            Utils::interpolate(state[2], bowPos - 1, alpha),
            Utils::interpolate(state[2], bowPos, alpha),
            Utils::interpolate(state[2], bowPos + 1, alpha),
    };

    auto b = coeffs[0] * velocity +
             coeffs[1] * uB[2] +
             coeffs[2] * uBPrev[1] +
             coeffs[3] * (uB[1] + uB[3]) +
             coeffs[4] * (uBPrev[0] + uBPrev[2]) +
             coeffs[5] * (uB[0] + uB[4]);

    auto vRel = 0.0, vRelPrev = 0.0;
    auto nrScaledForce = force / (resonatorParameters.rho * p.A);

    for (unsigned int i = 0; i < MAX_NR_ITERATIONS; ++i) {
        auto vRelPrevSq = pow(vRelPrev, 2);
        auto nr2 = phi1 * nrScaledForce * exp(-a * vRelPrevSq);

        vRel = vRelPrev -
               (nr1 * vRelPrev + nr2 * vRelPrev + b) /
               (nr1 + nr2 * (1 - 2 * a * vRelPrevSq));
        // threshold check
        if (fabs(vRel - vRelPrev) < NR_TOLERANCE) {
            break;
        }
        vRelPrev = vRel;
    }

    auto excitation = excitationCoefficient * force * vRel * exp(-a * pow(vRel, 2));

    // Apply the excitation.
    Utils::extrapolate(state[0], bowPos, alpha, p.h, -excitation);
}

void Bow::stopExcitation() {
    Exciter::stopExcitation();
}
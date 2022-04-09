//
// Created by Tommy Rushton on 08/04/2022.
//

#include <cmath>
#include "Bow.h"

//Bow::Bow(Resonator::ResonatorParameters &parameters) : Exciter(parameters) {}

void Bow::setFriction(FType friction) {
    a = friction;
}

void Bow::initialiseExcitation(float excitationPosition,
                               float excitationForce,
                               float excitationVelocity) {
    Exciter::initialiseExcitation(excitationPosition, excitationForce, excitationVelocity);

    auto p = resonatorParameters.derived;
    coeffs = {
            2 * (1 / p.k + p.sigma0),
            2 * (p.cSq / p.hSq - 1 / p.kSq + 3 * p.kappaSq / p.hSqSq + 2 * p.sigma1 / p.k * p.hSq),
            2 * (1 / p.kSq - 2 * p.sigma1 / p.k * p.hSq),
            -(p.cSq / p.hSq + 4 * p.kappaSq / p.hSqSq + 2 * p.sigma1 / p.k * p.hSq),
            2 * p.sigma1 / p.k * p.hSq,
            p.kappaSq / p.hSqSq
    };
}

void Bow::applyExcitation(std::vector<double *> &state) {
    auto alpha = modf(position, nullptr);
}

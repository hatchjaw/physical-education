//
// Created by Tommy Rushton on 07/04/2022.
//

#include "Exciter.h"

Exciter::Exciter(ModelParameters &parameters) : resonatorParameters(parameters) {}

void Exciter::startExcitation(float excitationPosition,
                              float excitationForce,
                              float excitationVelocity) {
    position = excitationPosition;
    force = excitationForce;
    velocity = excitationVelocity;
    isExciting = true;
}

void Exciter::stopExcitation() {
    force = 0.0;
    velocity = 0.0;
    isExciting = false;
}

void Exciter::setVelocity(float velocityToUse) {
    velocity = velocityToUse;
}

void Exciter::setForce(float forceToUse) {
    force = forceToUse;
}

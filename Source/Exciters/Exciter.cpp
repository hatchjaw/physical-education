//
// Created by Tommy Rushton on 07/04/2022.
//

#include "Exciter.h"

void Exciter::initialiseExcitation(float excitationPosition, float excitationForce, float excitationVelocity) {
    position = excitationPosition;
    force = excitationForce;
    velocity = excitationVelocity;
}

void Exciter::setVelocity(float velocityToUse) {
    velocity = velocityToUse;
}

void Exciter::setForce(float forceToUse) {
    force = forceToUse;
}

void Exciter::setNumGridPoints(unsigned int numGridPoints) {
    N = numGridPoints;
}

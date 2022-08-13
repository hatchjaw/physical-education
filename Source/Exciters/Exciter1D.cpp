//
// Created by Tommy Rushton on 12/08/2022.
//

#include "Exciter1D.h"

void Exciter1D::startExcitation(float excitationPosition,
                              float excitationForce,
                              float excitationVelocity) {
    position.x.set(excitationPosition);
    force = excitationForce;
    velocity = excitationVelocity;
    isExciting = true;
}
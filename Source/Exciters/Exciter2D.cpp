//
// Created by Tommy Rushton on 12/08/2022.
//

#include "Exciter2D.h"

void Exciter2D::startExcitation(std::pair<float, float> excitationPosition,
                                float excitationForce,
                                float excitationVelocity) {
    position.first.set(excitationPosition.first);
    position.second.set(excitationPosition.second);
    force = excitationForce;
    velocity = excitationVelocity;
    isExciting = true;
}

//
// Created by Tommy Rushton on 12/08/2022.
//

#include "Exciter2D.h"

void Exciter2D::startExcitation(std::pair<float, float> excitationPosition,
                                float excitationForce,
                                float excitationVelocity) {
    position.x.set(excitationPosition.first);
    position.y.set(excitationPosition.second);
    force = excitationForce;
    velocity = excitationVelocity;
    isExciting = true;
}

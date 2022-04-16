//
// Created by Tommy Rushton on 12/04/2022.
//

#include "Pluck.h"

void Pluck::setupExcitation() {

}

void Pluck::startExcitation(
        float excitationPosition,
        float excitationForce,
        float excitationVelocity
) {
    Exciter::startExcitation(excitationPosition, excitationForce, excitationVelocity);
}

void Pluck::applyExcitation(std::vector<FType *> &state) {

}

void Pluck::stopExcitation() {
    Exciter::stopExcitation();
}

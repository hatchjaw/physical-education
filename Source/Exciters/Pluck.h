//
// Created by Tommy Rushton on 12/04/2022.
//

#ifndef PHYSICAL_EDUCATION_PLUCK_H
#define PHYSICAL_EDUCATION_PLUCK_H


#include "Exciter.h"

class Pluck : public Exciter {
public:
    void setupExcitation() override;

    void startExcitation(float excitationPosition,
                         float excitationForce,
                         float excitationVelocity) override;

protected:
    void applyExcitation(std::vector<FType *> &state) override;

    void stopExcitation() override;
};


#endif //PHYSICAL_EDUCATION_PLUCK_H

//
// Created by Tommy Rushton on 12/08/2022.
//

#ifndef PHYSICAL_EDUCATION_EXCITER2D_H
#define PHYSICAL_EDUCATION_EXCITER2D_H


#include "Exciter.h"
#include "../Parameters/SmoothedParameter.h"
#include "../Grid/SpatialDomain.h"

class Exciter2D : public Exciter {
    using Exciter::Exciter;
public:
    virtual void startExcitation(std::pair<float, float> excitationPosition,
                                 float excitationForce,
                                 float excitationVelocity);

protected:
    std::pair<SmoothedParameter<float>, SmoothedParameter<float>> position{0.f, 0.f};

    friend class Resonator;
};


#endif //PHYSICAL_EDUCATION_EXCITER2D_H

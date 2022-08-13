//
// Created by Tommy Rushton on 12/08/2022.
//

#ifndef PHYSICAL_EDUCATION_EXCITER1D_H
#define PHYSICAL_EDUCATION_EXCITER1D_H


#include "Exciter.h"

class Exciter1D : public Exciter {
    using Exciter::Exciter;
public:
    virtual void startExcitation(
            float excitationPosition,
            float excitationForce,
            float excitationVelocity
    );
};


#endif //PHYSICAL_EDUCATION_EXCITER1D_H

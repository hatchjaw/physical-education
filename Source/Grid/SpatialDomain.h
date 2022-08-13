//
// Created by Tommy Rushton on 13/08/2022.
//

#ifndef PHYSICAL_EDUCATION_SPATIALDOMAIN_H
#define PHYSICAL_EDUCATION_SPATIALDOMAIN_H

#include "../Parameters/SmoothedParameter.h"

struct ContinuousPosition {
    ContinuousPosition() : x{0.f}, y{0.f} {}

    SmoothedParameter<float> x, y;
};

struct DiscretePosition {
    unsigned int l, m;
};

#endif //PHYSICAL_EDUCATION_SPATIALDOMAIN_H

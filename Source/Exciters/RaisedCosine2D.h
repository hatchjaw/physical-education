//
// Created by Tommy Rushton on 12/08/2022.
//

#ifndef PHYSICAL_EDUCATION_RAISEDCOSINE2D_H
#define PHYSICAL_EDUCATION_RAISEDCOSINE2D_H


#include "Exciter2D.h"
#include "../Grid/SpatialDomain.h"

class RaisedCosine2D : public Exciter2D {
    using Exciter2D::Exciter2D;
public:
    void setupExcitation() override {}

    void startExcitation(std::pair<float, float> excitationPosition,
                         float excitationForce,
                         float excitationVelocity) override;

protected:
    void applyExcitation(std::vector<FType *> &state) override;

private:
    const float WIDTH_SCALAR{.25f}, DURATION_SCALAR{10.f}, FORCE_SCALAR{.5f};
    unsigned int durationSamples{0}, sampleCount{0}, width{0};
    DiscretePosition start{0, 0};
};


#endif //PHYSICAL_EDUCATION_RAISEDCOSINE2D_H

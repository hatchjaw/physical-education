//
// Created by Tommy Rushton on 07/04/2022.
//

#ifndef PHYSICAL_EDUCATION_RAISEDCOSINE_H
#define PHYSICAL_EDUCATION_RAISEDCOSINE_H


#include "Exciter.h"
#include "../Resonators/Resonator.h"

class RaisedCosine : public Exciter {
public:
    using Exciter::Exciter;

    void setupExcitation() override {}

    void startExcitation(float excitationPosition,
                         float excitationForce,
                         float excitationVelocity) override;

    void setWidth(double normalisedWidth);

protected:
    void applyExcitation(std::vector<double *> &state) override;

    void stopExcitation() override;
private:
    const float WIDTH_SCALAR{.15f}, DURATION_SCALAR{10.f}, FORCE_SCALAR{.000005f};
    unsigned int width{0}, start{0}, durationSamples{0}, sampleCount{0};
};


#endif //PHYSICAL_EDUCATION_RAISEDCOSINE_H

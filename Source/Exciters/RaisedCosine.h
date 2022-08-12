//
// Created by Tommy Rushton on 07/04/2022.
//

#ifndef PHYSICAL_EDUCATION_RAISEDCOSINE_H
#define PHYSICAL_EDUCATION_RAISEDCOSINE_H


#include "Exciter1D.h"
#include "../Resonators/Resonator.h"

class RaisedCosine : public Exciter1D {
    using Exciter1D::Exciter1D;
public:
    void setupExcitation() override {}

    void startExcitation(float excitationPosition,
                         float excitationForce,
                         float excitationVelocity) override;

    void setWidth(double normalisedWidth);

protected:
    void applyExcitation(std::vector<FType *> &state) override;

    void stopExcitation() override;
private:
    const float WIDTH_SCALAR{.2f}, DURATION_SCALAR{10.f}, FORCE_SCALAR{2e-6f};
    unsigned int width{0}, start{0}, durationSamples{0}, sampleCount{0};
};


#endif //PHYSICAL_EDUCATION_RAISEDCOSINE_H

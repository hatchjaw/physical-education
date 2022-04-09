//
// Created by Tommy Rushton on 07/04/2022.
//

#ifndef PHYSICAL_EDUCATION_RAISEDCOSINE_H
#define PHYSICAL_EDUCATION_RAISEDCOSINE_H


#include "Exciter.h"
#include "../Resonators/Resonator.h"

class RaisedCosine : public Exciter {
public:
//    using Exciter::Exciter;
    explicit RaisedCosine(Resonator::ResonatorParameters &parameters);

    void initialiseExcitation(float excitationPosition,
                              float excitationForce,
                              float excitationVelocity) override;

    void setWidth(double normalisedWidth);

protected:
    void applyExcitation(std::vector<double *> &state) override;

private:
    const float WIDTH_SCALAR{.25f}, DURATION_SCALAR{8.f}, FORCE_SCALAR{.005f};
    unsigned int width{0}, start{0}, durationSamples{1}, sampleCount{1};
    Resonator::ResonatorParameters &resonatorParameters;
};


#endif //PHYSICAL_EDUCATION_RAISEDCOSINE_H

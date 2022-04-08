//
// Created by Tommy Rushton on 07/04/2022.
//

#ifndef PHYSICAL_EDUCATION_RAISEDCOSINE_H
#define PHYSICAL_EDUCATION_RAISEDCOSINE_H


#include "Exciter.h"

class RaisedCosine : public Exciter {
public:
    void initialiseExcitation(float excitationPosition,
                              float excitationForce,
                              float excitationVelocity) override;

    void setWidth(double normalisedWidth);

protected:
    void applyExcitation(std::vector<double *> &state) override;

private:
    const float WIDTH_SCALAR{.15f}, DURATION_SCALAR{8.f}, FORCE_SCALAR{.025f};
    unsigned int width{0}, start{0}, durationSamples{1}, sampleCount{1};
};


#endif //PHYSICAL_EDUCATION_RAISEDCOSINE_H

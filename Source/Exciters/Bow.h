//
// Created by Tommy Rushton on 08/04/2022.
//

#ifndef PHYSICAL_EDUCATION_BOW_H
#define PHYSICAL_EDUCATION_BOW_H


#include "Exciter.h"
#include "../Resonators/Resonator.h"

class Bow : public Exciter {
public:
//    using Exciter::Exciter;
//    Bow(Resonator::ResonatorParameters &parameters);

    void initialiseExcitation(float excitationPosition,
                              float excitationForce,
                              float excitationVelocity) override;

    void setFriction(FType friction);

protected:
    void applyExcitation(std::vector<double *> &state) override;

private:
    const unsigned int MAX_NR_ITERATIONS{100};
    const double TOLERANCE{1e-6};
    /**
     * Friction model free parameter.
     */
    FType a{0.0};
    /**
     * Derived coefficients.
     */
    std::vector<double> coeffs;

    Resonator::ResonatorParameters &resonatorParameters;
};


#endif //PHYSICAL_EDUCATION_BOW_H

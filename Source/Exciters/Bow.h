//
// Created by Tommy Rushton on 08/04/2022.
//

#ifndef PHYSICAL_EDUCATION_BOW_H
#define PHYSICAL_EDUCATION_BOW_H


#include "Exciter.h"
#include "../Resonators/Resonator.h"

class Bow : public Exciter {
public:
    using Exciter::Exciter;

    void setupExcitation() override;

    void startExcitation(float excitationPosition,
                         float excitationForce,
                         float excitationVelocity) override;

    void setFriction(FType friction);

protected:
    void applyExcitation(std::vector<FType *> &state) override;

    void stopExcitation() override;

private:
    const unsigned int MAX_NR_ITERATIONS{100};
    const double NR_TOLERANCE{1e-7};
    const float FORCE_SCALAR{.2f}, VELOCITY_SCALAR{.2f};
    /**
     * Friction model free parameter.
     */
    FType a{100.0};
    /**
     * Derived coefficients.
     */
    std::vector<FType> coeffs;
    FType phi1{0.0}, nr1{0.0}, excitationCoefficient{0.0};
};


#endif //PHYSICAL_EDUCATION_BOW_H

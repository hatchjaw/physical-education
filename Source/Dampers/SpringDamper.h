//
// Created by Tommy Rushton on 24/04/2022.
//

#ifndef PHYSICAL_EDUCATION_SPRINGDAMPER_H
#define PHYSICAL_EDUCATION_SPRINGDAMPER_H


#include "../Parameters/ModelParameters.h"
#include "../Parameters/SmoothedParameter.h"

class SpringDamper {
public:
    explicit SpringDamper(ModelParameters &parameters);

    void setupCollision();

private:
    void setLinearOscillatorFrequency(FType);

    void setNonlinearOscillatorFrequency(FType);

    void setLoss(FType);

    void setPosition(float);

    void applyCollision(std::vector<FType *> &state);

    SmoothedParameter<float> position{.5f};
    SmoothedParameter<FType> omega0{100.}, omega1{1000.}, sigmaP{.001};

    ModelParameters &resonatorParameters;

    /**
     * Derived coefficients.
     */
    std::vector<FType> coeffs;
    FType collisionForceCoefficient{0.0};

    friend class Resonator;
};


#endif //PHYSICAL_EDUCATION_SPRINGDAMPER_H

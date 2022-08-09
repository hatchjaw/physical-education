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

    void setupInteraction();

private:
    void setLinearOscillatorFrequency(FType);

    void setNonlinearOscillatorFrequency(FType);

    void setLoss(FType);

    void setPosition(float);

    void applyInteraction(std::vector<FType *> &state);

    SmoothedParameter<float> position{0.f};
    SmoothedParameter<FType> omega0{0.}, omega1{0.}, sigmaP{0.};

    ModelParameters &resonatorParameters;

    /**
     * Derived coefficients.
     */
    std::vector<FType> coeffs;
    FType forceCoefficient{0.0};

    friend class Resonator;
};


#endif //PHYSICAL_EDUCATION_SPRINGDAMPER_H

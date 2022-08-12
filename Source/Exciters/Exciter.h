//
// Created by Tommy Rushton on 07/04/2022.
//

#ifndef PHYSICAL_EDUCATION_EXCITER_H
#define PHYSICAL_EDUCATION_EXCITER_H

#include <vector>
#include "../Parameters/ModelParameters.h"
#include "../Parameters/SmoothedParameter.h"
// No need to include Resonator.h for the friend class declaration, as this
// header has been included from that very file.
// Including it confuses the compiler.

class Exciter {
public:
    explicit Exciter(ModelParameters &parameters);

    virtual void setupExcitation() = 0;

    float getVelocity() const;

    float getForce() const;

protected:
    void setForce(float);

    void setVelocity(float);

    virtual void applyExcitation(std::vector<FType *> &state) = 0;

    virtual void stopExcitation();

    float force{0.f}, velocity{0.f};

    bool isExciting{false};

    ModelParameters &resonatorParameters;

    friend class Resonator;

    friend class DynamicResonator;
};


#endif //PHYSICAL_EDUCATION_EXCITER_H

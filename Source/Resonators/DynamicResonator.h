//
// Created by Tommy Rushton on 09/08/2022.
//

#ifndef PHYSICAL_EDUCATION_DYNAMICRESONATOR_H
#define PHYSICAL_EDUCATION_DYNAMICRESONATOR_H


#include "Resonator.h"

class DynamicResonator : public Resonator {
    using Resonator::Resonator;
public:
    /**
     * Get a reference to the resonator's 'current' state vector.
     * @return The current state vector.
     */
    std::vector<FType> &getStaticState();

protected:
    void initialiseState() override;

    void advanceTimestep() override;

    virtual void doDisplacementCorrection() = 0;

    /**
     * State pointers for the fixed part of the system.
     */
    std::vector<FType *> w;
    std::vector<std::vector<FType>> wStates;

    FType NFrac{0.}, alpha, sigmaCkp, sigmaCkm, epsilon{0.}, restoringForce;
    unsigned int NPrev{0}, Mu{0}, Mw{0};
};


#endif //PHYSICAL_EDUCATION_DYNAMICRESONATOR_H

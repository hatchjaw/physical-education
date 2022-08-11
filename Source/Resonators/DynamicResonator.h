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
     * Get a reference to the static part of the system.
     * @return
     */
    std::vector<FType> &getStaticState();

    void initialiseModel(FType sampleRate) override;

    void updateState() override;

    void setTension(float tension);

protected:
    void initialiseState() override;

    void advanceTimestep() override;

    virtual void doDisplacementCorrection() = 0;

    FType getOutputAtPosition(unsigned long outputPositionIndex) override;

    /**
     * State pointers for the static part of the system.
     */
    std::vector<FType *> w;
    std::vector<std::vector<FType>> wStates;

    /**
     * Grid dimensions
     */
    unsigned int NPrev{0}, Mu{0}, Mw{0};
    FType NFrac{0.}, alpha{0.};
    /**
     * Displacement correction parameters
     */
    FType sigmaC{1.}, sigmaCkp{0.}, sigmaCkm{0.}, epsilon{0.}, restoringForce{0.};
};


#endif //PHYSICAL_EDUCATION_DYNAMICRESONATOR_H

//
// Created by Tommy Rushton on 11/08/2022.
//

#ifndef PHYSICAL_EDUCATION_DYNAMICRESONATOR2D_H
#define PHYSICAL_EDUCATION_DYNAMICRESONATOR2D_H

#include "DynamicResonator.h"

class DynamicResonator2D : public DynamicResonator {
//    using DynamicResonator::DynamicResonator;
public:
    DynamicResonator2D(std::tuple<unsigned int, unsigned int, unsigned int> stencilLMN, Exciter *exciterToUse);

    void setLength(FType length) override;

    void setDimensions(std::pair<FType, FType> dimensions);

    void setOutputPositions(std::vector<std::pair<FType, FType>> outputPositions);

    void setOutputPosition(unsigned long positionIndex, std::pair<FType, FType> normalisedPosition);

protected:
    void initialiseState() override;

    void initialiseModel(FType sampleRate) override;

    FType getOutputAtPosition(unsigned long outputPositionIndex) override;

    void advanceTimestep() override;

    unsigned int Nx{0}, NxPrev{0}, Ny{0}, NyPrev{0};
    FType Lx{0.}, Ly{0.}, NxFrac{0.}, NyFrac{0.}, alphaX{0.}, alphaY{0.};

    /**
     * State vectors for the static parts of the system.
     */
    std::vector<FType *> v;
    std::vector<std::vector<FType>> vStates;
    std::vector<FType *> z;
    std::vector<std::vector<FType>> zStates;

    std::tuple<unsigned int, unsigned int, unsigned int> stencilDimensions;

    /**
     * Positions from which to take displacement for output.
     */
    std::vector<std::pair<FType, FType>> normalisedOutputPositions;
};


#endif //PHYSICAL_EDUCATION_DYNAMICRESONATOR2D_H

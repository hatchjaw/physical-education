//
// Created by Tommy Rushton on 09/08/2022.
//

#ifndef PHYSICAL_EDUCATION_DYNAMIC1DWAVE_H
#define PHYSICAL_EDUCATION_DYNAMIC1DWAVE_H


#include "DynamicResonator.h"

class Dynamic1dWave : public DynamicResonator {
public:
    explicit Dynamic1dWave(Exciter *exciterToUse = nullptr);

    FType getOutputScalar() override;

    void setTension(FType tension);

    void setDensity(FType density);

    void setRadius(FType radius);

protected:
    void setDerivedParameters() override;

    void computeCoefficients() override;

    void computeScheme() override;

    void doDisplacementCorrection() override;
};


#endif //PHYSICAL_EDUCATION_DYNAMIC1DWAVE_H

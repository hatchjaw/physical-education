//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef PHYSICAL_EDUCATION_STIFFSTRING_H
#define PHYSICAL_EDUCATION_STIFFSTRING_H


#include "Resonator.h"
#include "../Exciters/RaisedCosine.h"

class StiffString : public Resonator {
public:
    explicit StiffString(Exciter *exciterToUse = nullptr);

    void setDensity(FType density);

    void setRadius(FType radius);

    void setTension(FType tension);

    void setYoungsModulus(FType youngsModulus);

    FType getOutputScalar() override;

private:
    const FType OUTPUT_SCALAR{1e3};

    void setDerivedParameters();

    void computeCoefficients() override;

    void computeScheme() override;
};


#endif //PHYSICAL_EDUCATION_STIFFSTRING_H

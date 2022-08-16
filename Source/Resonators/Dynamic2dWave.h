//
// Created by Tommy Rushton on 11/08/2022.
//

#ifndef PHYSICAL_EDUCATION_DYNAMIC2DWAVE_H
#define PHYSICAL_EDUCATION_DYNAMIC2DWAVE_H


#include "DynamicResonator2D.h"

class Dynamic2dWave : public DynamicResonator2D {
public:
    explicit Dynamic2dWave(Exciter *exciterToUse = nullptr);

    FType getOutputScalar() override;

    void setTension(FType tension, bool force = false);

    void setDensity(FType density, bool force = false);

    void setThickness(FType radius, bool force = false);

    ContinuousPosition getDimensions();

protected:
    void computeDerivedParameters() override;

    void computeCoefficients() override;

    void computeScheme() override;

    void doDisplacementCorrection() override;

    void adjustGridDimensions() override;

    void updateState() override;

private:
    int rateLimit{10}, rateCounter{0};
};


#endif //PHYSICAL_EDUCATION_DYNAMIC2DWAVE_H

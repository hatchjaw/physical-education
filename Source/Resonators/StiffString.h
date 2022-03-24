//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef NIME_PM_STIFFSTRING_H
#define NIME_PM_STIFFSTRING_H


#include "Resonator.h"

class StiffString : public Resonator {
public:
    StiffString();

    void updateState() override;

    void setWavespeed(FType newWavespeed);

    void setStiffness(FType newStiffness);

private:
    void computeCoefficients() override;

    /**
     * Model parameters. Wave propagation speed (m/s), combined string stiffness parameter.
     */
    FType c{700.}, kappa{9.};
};


#endif //NIME_PM_STIFFSTRING_H

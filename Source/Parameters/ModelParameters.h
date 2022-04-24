//
// Created by Tommy Rushton on 09/04/2022.
//

#ifndef PHYSICAL_EDUCATION_MODELPARAMETERS_H
#define PHYSICAL_EDUCATION_MODELPARAMETERS_H

#include "../Utils.h"

struct ModelParameters {
    struct DerivedParameters {
        /**
         * Timestep (s)
         */
        FType k{0.0}, kSq{0.0};
        /**
         * Grid spacing (m)
         */
        FType h{0.0}, hSq{0.0}, hSqSq{0.0};
        /**
         * Wavespeed (m/s)
         */
        FType c{0.0}, cSq{0.0};
        /**
         * Stiffness
         */
        FType kappa{0.0}, kappaSq{0.0};
        /**
         * Damping coefficients
         */
        FType sigma0{0.0}, sigma1{0.0};
        /**
         * Cross-sectional area (m^2)
         */
        FType A{0.0};
        /**
         * Area moment of inertia (m^4)
         */
        FType I{0.0};
        /**
         * Number of grid points
         */
        unsigned int N{0};
        /**
         * Combined coefficients
         */
        FType lambda{0.0}, mu{0.0};
        /**
         * Just for reference/debugging; should be close to 1
         */
        FType lambdaSqPlus4MuSq{0.0};
        /**
         * 1 + sigma0*k
         * Useful for setting coefficients, e.g. for damping/collisions.
         */
        FType schemeDivisor{1.0};
        /**
         * Scheme coefficients without the divisor. Useful for collisions and
         * perhaps other situations where the divisor may differ from what it is
         * for the resonator.
         */
        std::vector<FType> rawCoeffs;
    };
    /**
     * Frequency-independent decay time
     */
    FType T60_0{0.0};
    /**
     * Frequency-dependent decay...
     * TODO: needs to be calculated differently.
     * NB. for stability, T60_1 should be greater than T60_0.
     */
    FType T60_1{0.0};
    /**
     * Density (kg/m^3)
     */
    FType rho{0.0};
    /**
     * Cross-sectional radius (m)
     */
    FType r{0.0};
    /**
     * Tension (N)
     */
    FType T{0.0};
    /**
     * Young's modulus (Pa)
     */
    FType E{0.0};
    DerivedParameters derived;
};

#endif //PHYSICAL_EDUCATION_MODELPARAMETERS_H

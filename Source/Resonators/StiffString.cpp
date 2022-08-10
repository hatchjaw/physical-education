//
// Created by Tommy Rushton on 23/03/2022.
//

#include <cmath>
#include "StiffString.h"

StiffString::StiffString(Exciter *exciterToUse) :
        Resonator(std::make_pair(5, 3), exciterToUse) {
}

void StiffString::setDensity(FType density) {
    parameters.rho = density;
    setDerivedParameters();
}

void StiffString::setRadius(FType radius) {
    parameters.r = radius;
    setDerivedParameters();
}

void StiffString::setTension(FType tension) {
    parameters.T = tension;
    setDerivedParameters();
}

void StiffString::setYoungsModulus(FType youngsModulus) {
    parameters.E = youngsModulus;
    setDerivedParameters();
}

void StiffString::setDerivedParameters() {
    if (parameters.r > 0.0) {
        auto p = &parameters.derived;
        p->A = M_PI * pow(parameters.r, 2);
        p->I = 0.25 * M_PI * pow(parameters.r, 4);

        auto rhoA = parameters.rho * p->A;
        if (rhoA > 0.0) {
            p->cSq = parameters.T / rhoA;
            p->c = sqrt(p->cSq);
            p->kappaSq = parameters.E * p->I / rhoA;
            p->kappa = sqrt(p->kappaSq);

            auto sigmas = t60ToSigma(parameters.T60_0, parameters.T60_1);
            p->sigma0 = sigmas.first;
            p->sigma1 = sigmas.second;
        }
    }
}

void StiffString::computeCoefficients() {
    // TODO: throw exception if derived parameters aren't set by now.
    auto p = &parameters.derived;
    // Grid-spacing, from stability condition.
    auto hh = p->cSq * p->kSq + (4 * p->sigma1 * p->k);
    auto hhh = sqrt(pow(hh, 2) + (16 * p->kappaSq * p->kSq));
    p->h = sqrt((hh + hhh) / 2);
    // Calculate the closest integer N such that h ≥ [stability condition]
    p->N = static_cast<unsigned int>(floor(L / p->h));
    // Recalculate grid-spacing from final N.
    p->h = L / static_cast<float>(p->N);
    p->hSq = pow(p->h, 2);
    p->hSqSq = pow(p->hSq, 2);

    // Compute model coefficients.
    p->lambda = (p->k * p->c) / p->h;
    auto lambdaSq = pow(p->lambda, 2);
    p->mu = (p->kappa * p->k) / p->hSq;
    auto muSq = pow(p->mu, 2);
    auto S = (2 * p->sigma1 * p->k) / p->hSq;

    p->lambdaSqPlus4MuSq = lambdaSq + 4 * muSq;

    coeffs = {
            // u_l^n
            2 - (2 * lambdaSq) - (6 * muSq) - (2 * S),
            // u_l^n, boundaries
            2 - (2 * lambdaSq) - (5 * muSq) - (2 * S),
            // u_l^{n-1}
            (p->sigma0 * p->k) - 1 + (2 * S),
            // u_{l±2}^n
            -muSq,
            // u_{l±1}^n
            lambdaSq + (4 * muSq) + S,
            // u_{l±1}^{n-1}
            -S,
    };

    p->rawCoeffs = coeffs;
    p->schemeDivisor = 1 + (p->sigma0 * p->k);

    // Scale all coefficients by the divisor to the FDS
    for (auto &coeff: coeffs) {
        coeff /= p->schemeDivisor;
    }
}

void StiffString::computeScheme() {
    // Handle boundary conditions (simply-supported)
    unsigned int l = 1;
    u[0][l] = coeffs[1] * u[1][l] +
              coeffs[2] * u[2][l] +
              coeffs[3] * u[1][l + 2] +
              coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
              coeffs[5] * (u[2][l + 1] + u[2][l - 1]);

    l = parameters.derived.N - 1;
    u[0][l] = coeffs[1] * u[1][l] +
              coeffs[2] * u[2][l] +
              coeffs[3] * u[1][l - 2] +
              coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
              coeffs[5] * (u[2][l + 1] + u[2][l - 1]);

    // Update all the non-boundary positions.
    for (l = 2; l < parameters.derived.N - 1; ++l) {
        u[0][l] = coeffs[0] * u[1][l] +
                  coeffs[2] * u[2][l] +
                  coeffs[3] * (u[1][l + 2] + u[1][l - 2]) +
                  coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
                  coeffs[5] * (u[2][l + 1] + u[2][l - 1]);
    }

    // Circular string...
//    unsigned int N = parameters.derived.N;
//    auto NoverTwo = static_cast<unsigned int>(floor(N / 2));
//    unsigned int l = 0;
//    u[0][l] = coeffs[0] * u[1][l] +
//              coeffs[2] * u[2][l] +
//              coeffs[3] * (u[1][l + 2] + u[1][N - 1]) +
//              coeffs[4] * (u[1][l + 1] + u[1][N]) +
//              coeffs[5] * (u[2][l + 1] + u[2][N]);
//
//    l = 1;
//    u[0][l] = coeffs[0] * u[1][l] +
//              coeffs[2] * u[2][l] +
//              coeffs[3] * (u[1][l + 2] + u[1][N]) +
//              coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
//              coeffs[5] * (u[2][l + 1] + u[2][l - 1]);
//
//    for (l = 2; l < NoverTwo; ++l) {
//        u[0][l] = coeffs[0] * u[1][l] +
//                  coeffs[2] * u[2][l] +
//                  coeffs[3] * (u[1][l + 2] + u[1][l - 2]) +
//                  coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
//                  coeffs[5] * (u[2][l + 1] + u[2][l - 1]);
//    }
//
//    for (l = NoverTwo + 1; l < N - 1; ++l) {
//        u[0][l] = coeffs[0] * u[1][l] +
//                  coeffs[2] * u[2][l] +
//                  coeffs[3] * (u[1][l + 2] + u[1][l - 2]) +
//                  coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
//                  coeffs[5] * (u[2][l + 1] + u[2][l - 1]);
//    }
//
//    l = N - 1;
//    u[0][l] = coeffs[0] * u[1][l] +
//              coeffs[2] * u[2][l] +
//              coeffs[3] * (u[1][0] + u[1][l - 2]) +
//              coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
//              coeffs[5] * (u[2][l + 1] + u[2][l - 1]);
//
//    l = N;
//    u[0][l] = coeffs[0] * u[1][l] +
//              coeffs[2] * u[2][l] +
//              coeffs[3] * (u[1][1] + u[1][l - 2]) +
//              coeffs[4] * (u[1][0] + u[1][l - 1]) +
//              coeffs[5] * (u[2][0] + u[2][l - 1]);
}

FType StiffString::getOutputScalar() {
    switch (outputMode) {
        case DISPLACEMENT:
            return .75e3;
        case VELOCITY:
            return 1;
    }
}

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
    auto nu = (2 * p->sigma1 * p->k) / p->hSq;

    p->lambdaSqPlus4MuSq = lambdaSq + 4 * muSq;

    coeffs = {
            // u_l^n
            2 - (2 * lambdaSq) - (6 * muSq) - (2 * nu),
            // u_l^n, boundaries
            2 - (2 * lambdaSq) - (5 * muSq) - (2 * nu),
            // u_l^{n-1}
            (p->sigma0 * p->k) - 1 + (2 * nu),
            // u_{l±2}^n
            -muSq,
            // u_{l±1}^n
            lambdaSq + (4 * muSq) + nu,
            // u_{l±1}^{n-1}
            -nu,
    };

    // Scale all coefficients by the divisor to the FDS
    for (auto &coeff: coeffs) {
        coeff /= 1 + (p->sigma0 * p->k);
    }
}

void StiffString::computeScheme() {
    // Handle boundary conditions (simply-supported)
    unsigned int l = 1;
    u[2][l] = coeffs[1] * u[1][l] +
              coeffs[2] * u[0][l] +
              coeffs[3] * u[1][l + 2] +
              coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
              coeffs[5] * (u[0][l + 1] + u[0][l - 1]);

    l = parameters.derived.N - 1;
    u[2][l] = coeffs[1] * u[1][l] +
              coeffs[2] * u[0][l] +
              coeffs[3] * u[1][l - 2] +
              coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
              coeffs[5] * (u[0][l + 1] + u[0][l - 1]);

    // Update all the non-boundary positions.
    for (l = 2; l < parameters.derived.N - 1; ++l) {
        u[2][l] = coeffs[0] * u[1][l] +
                  coeffs[2] * u[0][l] +
                  coeffs[3] * (u[1][l + 2] + u[1][l - 2]) +
                  coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
                  coeffs[5] * (u[0][l + 1] + u[0][l - 1]);
    }
}

FType StiffString::getOutputScalar() {
    return OUTPUT_SCALAR;
}

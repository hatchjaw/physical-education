//
// Created by Tommy Rushton on 23/03/2022.
//

#include <cmath>
#include "StiffString.h"

StiffString::StiffString(Exciter *exciterToUse) :
        Resonator(std::make_pair(5, 3), exciterToUse) {
}

void StiffString::setWavespeed(FType newWavespeed) {
    c = newWavespeed;
}

void StiffString::setStiffness(FType newStiffness) {
    kappa = newStiffness;
}

void StiffString::computeCoefficients() {
    // Frequency independent damping.
    auto sigma0 = t60ToSigma(T60_0);
    // Frequency-dependent damping.
    auto sigma1 = t60ToSigma(T60_1);

    // Grid-spacing, from stability condition.
    auto hh = pow(c, 2) * pow(k, 2) + (4 * sigma1 * k);
    auto hhh = sqrt(pow(hh, 2) + (16 * pow(kappa, 2) * pow(k, 2)));
    auto h = sqrt((hh + hhh) / 2);
    // Calculate the closest integer N such that h ≥ [stability condition]
    N = static_cast<unsigned int>(floor(L / h));
    // Recalculate grid-spacing from final N.
    h = L / static_cast<float>(N);

    // Compute model coefficients.
    auto hSq = pow(h, 2);
    auto lambda = (k * c) / h;
    auto lambdaSq = pow(lambda, 2);
    auto mu = (kappa * k) / hSq;
    auto muSq = pow(mu, 2);
    auto nu = (2 * sigma1 * k) / hSq;

    coeffs = {
            // u_l^n
            2 - (2 * lambdaSq) - (6 * muSq) - (2 * nu),
            // u_l^n, boundaries
            2 - (2 * lambdaSq) - (5 * muSq) - (2 * nu),
            // u_l^{n-1}
            (sigma0 * k) - 1 + (2 * nu),
            // u_{l±2}^n
            -muSq,
            // u_{l±1}^n
            lambdaSq + (4 * muSq) + nu,
            // u_{l±1}^{n-1}
            -nu,
    };

    // Scale all coefficients by the divisor to the FDS
    for (auto &coeff: coeffs) {
        coeff /= 1 + (sigma0 * k);
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

    l = N - 1;
    u[2][l] = coeffs[1] * u[1][l] +
              coeffs[2] * u[0][l] +
              coeffs[3] * u[1][l - 2] +
              coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
              coeffs[5] * (u[0][l + 1] + u[0][l - 1]);

    // Update all the non-boundary positions.
    for (l = 2; l < N - 1; ++l) {
        u[2][l] = coeffs[0] * u[1][l] +
                  coeffs[2] * u[0][l] +
                  coeffs[3] * (u[1][l + 2] + u[1][l - 2]) +
                  coeffs[4] * (u[1][l + 1] + u[1][l - 1]) +
                  coeffs[5] * (u[0][l + 1] + u[0][l - 1]);
    }
}

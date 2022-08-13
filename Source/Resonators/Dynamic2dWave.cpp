//
// Created by Tommy Rushton on 11/08/2022.
//

#include "Dynamic2dWave.h"

Dynamic2dWave::Dynamic2dWave(Exciter *exciterToUse) :
        DynamicResonator2D({3, 3, 3}, exciterToUse) {
}

FType Dynamic2dWave::getOutputScalar() {
    switch (outputMode) {
        case DISPLACEMENT:
            return .1;
        case VELOCITY:
            return 1e-5;
    }
}

void Dynamic2dWave::setTension(FType tension, bool force) {
    parameters.T.set(tension, force);
}

void Dynamic2dWave::setDensity(FType density, bool force) {
    parameters.rho.set(density, force);
}

void Dynamic2dWave::setThickness(FType radius, bool force) {
    parameters.H.set(radius, force);
}

void Dynamic2dWave::computeDerivedParameters() {
    auto H = parameters.H.getNext();
    if (H > 0.0) {
        auto p = &parameters.derived;

        auto rhoH = parameters.rho.getNext() * H;

        if (rhoH > 0.0) {
            p->c = sqrt(parameters.T.getNext() / rhoH);
            p->cSq = pow(p->c, 2);

            auto sigmas = t60ToSigma(parameters.T60_0, parameters.T60_1);
            p->sigma0 = sigmas.first;
            p->sigma1 = sigmas.second;
        }
    }
}

void Dynamic2dWave::computeCoefficients() {
    auto p = &parameters.derived;

    // Grid-spacing, from stability condition.
    p->h = sqrt(2. * (p->cSq * p->kSq + 4 * p->sigma1 * p->k));
    p->hSq = pow(p->h, 2);

    NxFrac = Lx / p->h;
    NyFrac = Ly / p->h;

    double integralPart;
    alphaX = modf(NxFrac, &integralPart);
    Nx = static_cast<unsigned int>(integralPart);
    alphaY = modf(NyFrac, &integralPart);
    Ny = static_cast<unsigned int>(integralPart);

    // Compute model coefficients.
    p->lambda = (p->k * p->c) / p->h;
    auto lambdaSq = pow(p->lambda, 2);
    auto S = (2 * p->sigma1 * p->k) / p->hSq;

    coeffs = {
            // u_{l,m}^n
            2 * (1 - 2 * (lambdaSq + S)),
            // u_{l±1,m±1}^n
            lambdaSq + S,
            // u_{l,m}^{n-1}
            p->sigma0 * p->k - 1 + 4 * S,
            // u_{l±1,m±1}^{n-1}
            -S
    };

    p->rawCoeffs = coeffs;
    p->schemeDivisor = 1. + (p->sigma0 * p->k);

    // Scale all coefficients by the divisor to the FDS
    for (auto &coeff: coeffs) {
        coeff /= p->schemeDivisor;
    }
}

void Dynamic2dWave::computeScheme() {
    auto p = parameters.derived;
    unsigned int l, m;

    auto lWidth = p.Mxu + 1;
    for (m = 1; m < p.Myu - 1; ++m) {
        auto mc = m * lWidth, mm = (m - 1) * lWidth, mp = (m + 1) * lWidth;

        for (l = 1; l < p.Mxu - 1; ++l) {
            auto lm = mc + l, // u_{l,m}
            lmm1 = mm + l, // u_{l,m-1}
            lmp1 = mp + l, // u_{l,m+1}
            lm1m = lm - 1, // u_{l-1,m}
            lp1m = lm + 1; // u_{l+1,m}

            u[0][lm] = coeffs[0] * u[1][lm] +
                       coeffs[1] * (u[1][lp1m] + u[1][lm1m] + u[1][lmm1] + u[1][lmp1]) +
                       coeffs[2] * u[2][lm] +
                       coeffs[3] * (u[2][lp1m] + u[2][lm1m] + u[2][lmm1] + u[2][lmp1]);
        }
    }
}

void Dynamic2dWave::doDisplacementCorrection() {

}

void Dynamic2dWave::adjustGridDimensions() {
    auto p = parameters.derived;

    if (Nx != NxPrev) {
        auto diff = static_cast<int>(NxPrev) - static_cast<int>(Nx);
        jassert(abs(diff) == 1);

        p.Mxu = Nx - 1;
    }

    if (Ny != NyPrev) {
        auto diff = static_cast<int>(NyPrev) - static_cast<int>(Ny);
        jassert(abs(diff) == 1);

        p.Myu = Ny - 1;
    }

    // Store current N to check against next iteration.
    NxPrev = Nx;
    NyPrev = Ny;
}

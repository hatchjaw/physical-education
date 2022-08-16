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

    // VIRTUAL GRIDPOINTS
    // Create quadratic interpolators.
    auto axm{alphaX - 1.};
    auto axp{alphaX + 1.};
    std::vector<FType> ipx{-axm / axp, 1, axm / axp};
    auto aym{alphaY - 1.};
    auto ayp{alphaY + 1.};
    std::vector<FType> ipy{-aym / ayp, 1, aym / ayp};

    auto uWidth{p.Mxu + 1};
    unsigned int vWidth{2}, wWidth{p.Mxu + 1};

    for (unsigned int m = 1; m < p.Myu; ++m) {
        auto mc = m * uWidth;
        auto mm = mc - uWidth;
        auto mp = mc + uWidth;

        // Main chunk of u
        for (unsigned int l = 1; l < p.Mxu; ++l) {
            auto ulm = mc + l; // u_{l,m}
            auto ulmm1 = mm + l; // u_{l,m-1}
            auto ulmp1 = mp + l; // u_{l,m+1}
            auto ulm1m = ulm - 1; // u_{l-1,m}
            auto ulp1m = ulm + 1; // u_{l+1,m}

            u[0][ulm] = coeffs[0] * u[1][ulm] +
                        coeffs[1] * (u[1][ulp1m] + u[1][ulm1m] + u[1][ulmp1] + u[1][ulmm1]) +
                        coeffs[2] * u[2][ulm] +
                        coeffs[3] * (u[2][ulp1m] + u[2][ulm1m] + u[2][ulmp1] + u[2][ulmm1]);
        }

        /*
        // u-v interaction
        unsigned int l = p.Mxu;
        auto ulm = mc + l; // u_{l,m}
        auto ulmm1 = mm + l; // u_{l,m-1}
        auto ulmp1 = mp + l; // u_{l,m+1}
        auto ulm1m = ulm - 1; // u_{l-1,m}
        auto vlm = vWidth * m;
        auto vlp1m = vlm + 1;
        auto vlmm1 = vlm - vWidth;
        auto vlmp1 = vlm + vWidth;

        FType vgUV[2]{
                ipx[2] * u[1][ulm] + ipx[1] * v[1][vlm] + ipx[0] * v[1][vlp1m],
                ipx[2] * u[2][ulm] + ipx[1] * v[2][vlm] + ipx[0] * v[2][vlp1m]
        };

        u[0][ulm] = coeffs[0] * u[1][ulm] +
                    coeffs[1] * (vgUV[0] + u[1][ulm1m] + u[1][ulmp1] + u[1][ulmm1]) +
                    coeffs[2] * u[2][ulm] +
                    coeffs[3] * (vgUV[1] + u[2][ulm1m] + u[2][ulmp1] + u[2][ulmm1]);

        // v-u interaction
        FType vgVU[2]{
                ipx[0] * u[1][ulm1m] + ipx[1] * u[1][ulm] + ipx[2] * v[1][vlm],
                ipx[0] * u[2][ulm1m] + ipx[1] * u[2][ulm] + ipx[2] * v[2][vlm]
        };

        v[0][vlm] = coeffs[0] * v[1][vlm] +
                    coeffs[1] * (v[1][vlp1m] + vgVU[0] + v[1][vlmp1] + v[1][vlmm1]) +
                    coeffs[2] * v[2][vlm] +
                    coeffs[3] * (v[2][vlp1m] + vgVU[1] + v[2][vlmp1] + v[2][vlmm1]);
                    */
    }

    /*
    auto m = p.Myu;
    auto mc = m * uWidth;
    auto mm = (m - 1) * uWidth;

    for (unsigned int l = 1; l < p.Mxu; ++l) {
        // u-w interaction
        auto ulm = mc + l; // u_{l,m}
        auto ulmm1 = mm + l; // u_{l,m-1}
        auto ulm1m = ulm - 1; // u_{l-1,m}
        auto ulp1m = ulm + 1; // u_{l+1,m}
        auto wlm = l;
        auto wlmp1 = wlm + wWidth;
        auto wlm1m = wlm - 1;
        auto wlp1m = wlm + 1;

        FType vgUW[2] = {
                ipx[2] * u[1][ulm] + ipx[1] * w[1][wlm] + ipx[0] * w[1][wlmp1],
                ipx[2] * u[2][ulm] + ipx[1] * w[2][wlm] + ipx[0] * w[2][wlmp1]
        };

        u[0][ulm] = coeffs[0] * u[1][ulm] +
                    coeffs[1] * (u[1][ulp1m] + u[1][ulm1m] + vgUW[0] + u[1][ulmm1]) +
                    coeffs[2] * u[2][ulm] +
                    coeffs[3] * (u[2][ulp1m] + u[2][ulm1m] + vgUW[1] + u[2][ulmm1]);

        // w-u interaction
        FType vgWU[2] = {
                ipx[0] * u[1][ulm1m] + ipx[1] * u[1][ulm] + ipx[2] * w[1][wlm],
                ipx[0] * u[2][ulm1m] + ipx[1] * u[2][ulm] + ipx[2] * w[2][wlm]
        };

        w[0][ulm] = coeffs[0] * w[1][wlm] +
                    coeffs[1] * (w[1][wlp1m] + w[1][wlm1m] + w[1][wlmp1] + vgWU[0]) +
                    coeffs[2] * w[2][wlm] +
                    coeffs[3] * (w[2][wlp1m] + w[2][wlm1m] + w[2][wlmp1] + vgWU[1]);
    }


    // u-v-w interaction
    auto ulm = (p.Mxu + 1) * (p.Myu + 1) - 1;
    auto ulm1m = ulm - 1;
    auto ulmm1 = ulm - uWidth;
    auto vlm = p.Myu * vWidth; // v_{0,Myu}
    auto vlp1m = vlm + 1; // v_{1,Myu}
    auto vlmm1 = vlm - vWidth; // v_{0,Myu-1}
    auto wlm = p.Mxu; // w_{Mxu,0}
    auto wlmp1 = wlm + wWidth; // w_{Mxu,1}
    auto wlm1m = wlm - 1; // w_{Mxu-1,0}
    auto zlm = 0;
    auto zlmp1 = zlm + vWidth;
    auto zlp1m = zlm + 1;

    FType vgUV[2]{
            ipx[2] * u[1][ulm] + ipx[1] * v[1][vlm] + ipx[0] * v[1][vlp1m],
            ipx[2] * u[2][ulm] + ipx[1] * v[2][vlm] + ipx[0] * v[2][vlp1m]
    };
    FType vgVU[2]{
            ipx[0] * u[1][ulm1m] + ipx[1] * u[1][ulm] + ipx[2] * v[1][vlm],
            ipx[0] * u[2][ulm1m] + ipx[1] * u[2][ulm] + ipx[2] * v[2][vlm]
    };
    FType vgUW[2]{
            ipx[2] * u[1][ulm] + ipx[1] * w[1][wlm] + ipx[0] * w[1][wlmp1],
            ipx[2] * u[2][ulm] + ipx[1] * w[2][wlm] + ipx[0] * w[2][wlmp1]
    };
    FType vgWU[2]{
            ipx[0] * u[1][ulm1m] + ipx[1] * u[1][ulm] + ipx[2] * w[1][wlm],
            ipx[0] * u[2][ulm1m] + ipx[1] * u[2][ulm] + ipx[2] * w[2][wlm]
    };

    u[0][ulm] = coeffs[0] * u[1][ulm] +
                coeffs[1] * (vgUV[0] + u[1][ulm1m] + vgUW[0] + u[1][ulmm1]) +
                coeffs[2] * u[2][ulm] +
                coeffs[3] * (vgUV[1] + u[2][ulm1m] + vgUW[1] + u[2][ulmm1]);

    FType vgVZ[2]{
            ipx[2] * v[1][vlm] + ipx[1] * z[1][zlm] + ipx[0] * z[1][zlmp1],
            ipx[2] * v[2][vlm] + ipx[1] * z[2][zlm] + ipx[0] * z[2][zlmp1]
    };
    FType vgZV[2]{
            ipx[0] * v[1][vlmm1] + ipx[1] * v[1][vlm] + ipx[2] * z[1][zlm],
            ipx[0] * v[2][vlmm1] + ipx[1] * v[2][vlm] + ipx[2] * z[2][zlm]
    };
    FType vgWZ[2]{
            ipx[2] * w[1][wlm] + ipx[1] * z[1][zlm] + ipx[0] * z[1][zlp1m],
            ipx[2] * w[2][wlm] + ipx[1] * z[2][zlm] + ipx[0] * z[2][zlp1m]
    };
    FType vgZW[2]{
            ipx[0] * w[1][wlm1m] + ipx[1] * w[1][wlm] + ipx[2] * z[1][zlm],
            ipx[0] * w[2][wlm1m] + ipx[1] * w[2][wlm] + ipx[2] * z[2][zlm]
    };

    // v-u-z interaction
    v[0][vlm] = coeffs[0] * v[1][vlm] +
                coeffs[1] * (v[1][vlp1m] + vgVU[0] + vgVZ[0] + v[1][vlmm1]) +
                coeffs[2] * v[2][vlm] +
                coeffs[3] * (v[2][vlp1m] + vgVU[1] + vgVZ[1] + v[2][vlmm1]);

    // w-u-z interaction
    w[0][wlm] = coeffs[0] * w[1][wlm] +
                coeffs[1] * (vgWZ[0] + w[1][wlm1m] + w[1][wlmp1] + vgWU[0]) +
                coeffs[2] * w[2][wlm] +
                coeffs[3] * (vgWZ[1] + w[2][wlm1m] + w[2][wlmp1] + vgWU[1]);

    // z-v-w interaction
    z[0][zlm] = coeffs[0] * z[1][zlm] +
                coeffs[1] * (z[1][zlp1m] + vgZW[0] + z[1][zlmp1] + vgZV[0]) +
                coeffs[2] * z[2][zlm] +
                coeffs[3] * (z[2][zlp1m] + vgZW[1] + z[2][zlmp1] + vgZV[1]);
                */
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

    // Store current Nx, Ny to check against next iteration.
    NxPrev = Nx;
    NyPrev = Ny;
}

void Dynamic2dWave::doDisplacementCorrection() {
    /*
    auto p = parameters.derived;

    auto onemalphaxSigckp{(1 - alphaX) * sigmaCkp};
    auto onemalphaySigckp{(1 - alphaY) * sigmaCkp};
    auto ax{(p.h * onemalphaxSigckp) / (2 * (p.h * (alphaX + epsilon) + p.kSq * onemalphaxSigckp))};
    auto ay{(p.h * onemalphaySigckp) / (2 * (p.h * (alphaY + epsilon) + p.kSq * onemalphaySigckp))};
    auto hyp{sqrt(pow(ax, 2) + pow(ay, 2))};
    auto schemeCoeff{p.kSq / (p.h * (1 + p.sigma0 * p.k))};

    // u-v correction
    for (unsigned int m = 1; m < p.Myu; ++m) {
        auto vStart{2 * m},
                uEnd{(m + 1) * (p.Mxu + 1) - 1};
        auto etaUVNext{v[0][vStart] - u[0][uEnd]},
                etaUVPrev{v[2][vStart] - u[2][uEnd]},
                correctionFuv{ax * (restoringForce * etaUVPrev + etaUVNext) * schemeCoeff};
//        u(end, 1:end-1, next) = u(end, 1:end-1, next) + correctionFuv(1:end-1);
        u[0][uEnd] += correctionFuv;
//        v(1, 1:end-1, next) = v(1, 1:end-1, next) - correctionFuv(1:end-1);
        v[0][vStart] -= correctionFuv;
    }

    // u-w correction
    for (unsigned int l = 1; l < p.Mxu; ++l) {
        auto wStart{l};
        auto uEnd{p.Mxu * (p.Myu + 1) + l};
//        etaUWNext = w(:, 1, next) - u(:, end, next);
//        etaUWPrev = w(:, 1, prev) - u(:, end, prev);
        auto etaUWNext{w[0][wStart] - u[0][uEnd]};
        auto etaUWPrev{w[2][wStart] - u[2][uEnd]};
        auto correctionFuw{ay * (restoringForce * etaUWPrev + etaUWNext) * schemeCoeff};
//        u(1:end-1, end, next) = u(1:end-1, end, next) + correctionFuw(1:end-1);
        u[0][uEnd] += correctionFuw;
//        w(1:end-1, 1, next) = w(1:end-1, 1, next) - correctionFuw(1:end-1);
        w[0][wStart] -= correctionFuw;
    }

    auto uEnd{(p.Mxu + 1) * (p.Myu + 1) - 1};
    auto vEnd{2 * (p.Myu + 1)};
    auto wEnd{p.Mxu};

    auto etaUVNext{v[0][vEnd] - u[0][uEnd]};
    auto etaUVPrev{v[2][vEnd] - u[2][uEnd]};
    auto etaUWNext{w[0][wEnd] - u[0][uEnd]};
    auto etaUWPrev{w[2][wEnd] - u[2][uEnd]};
    auto etaUZNext{z[0][0] - u[0][uEnd]};
    auto etaUZPrev{z[2][0] - u[2][uEnd]};
    auto etaVZNext{z[0][0] - v[0][vEnd]};
    auto etaVZPrev{z[2][0] - v[2][vEnd]};
    auto etaWZNext{z[0][0] - w[0][wEnd]};
    auto etaWZPrev{z[2][0] - w[2][wEnd]};
    auto etaVWNext{w[0][wEnd] - v[0][vEnd]};
    auto etaVWPrev{w[2][wEnd] - v[2][vEnd]};
    auto correctionFuv{ax * (restoringForce * etaUVPrev + etaUVNext) * schemeCoeff};
    auto correctionFuw{ay * (restoringForce * etaUWPrev + etaUWNext) * schemeCoeff};
    auto correctionFuz{hyp * (restoringForce * etaUZPrev + etaUZNext) * schemeCoeff};
    auto correctionFvz{ay * (restoringForce * etaVZPrev + etaVZNext) * schemeCoeff};
    auto correctionFwz{ax * (restoringForce * etaWZPrev + etaWZNext) * schemeCoeff};
    auto correctionFvw{hyp * (restoringForce * etaVWPrev + etaVWNext) * schemeCoeff};

    // u-v-w-z correction
//    u(end, end, next) = u(end, end, next) + .5*(correctionFuv(end) + correctionFuw(end)) + correctionFuz;
    u[0][uEnd] += .5 * (correctionFuv + correctionFuw) + correctionFuz;

    // v-u-w correction
//    v(1, end, next) = v(1, end, next) - .5*(correctionFvz + correctionFuv(end)) - correctionFvw;
    v[0][vEnd] += -.5 * (correctionFvz + correctionFuv) - correctionFvw;

    // w-u-v correction
//    w(end, 1, next) = w(end, 1, next) - .5*(correctionFwz + correctionFuw(end)) + correctionFvw;
    w[0][wEnd] += -.5 * (correctionFwz + correctionFuw) + correctionFvw;

    // z-w-v-u correction
//    z(1, 1, next) = z(1, 1, next) + .5*(correctionFvz + correctionFwz) - correctionFuz;
    z[0][0] += .5 * (correctionFvz + correctionFwz) - correctionFuz;
     */
}

void Dynamic2dWave::updateState() {
    if (++rateCounter == rateLimit) {
        DynamicResonator::updateState();
        rateCounter = 0;
    }
}

ContinuousPosition Dynamic2dWave::getDimensions() {
    ContinuousPosition pos;
    pos.x.set(Lx, true);
    pos.y.set(Ly, true);
    return pos;
}

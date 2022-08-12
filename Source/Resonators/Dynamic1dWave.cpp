//
// Created by Tommy Rushton on 09/08/2022.
//

#include "Dynamic1dWave.h"

Dynamic1dWave::Dynamic1dWave(Exciter *exciterToUse) :
        DynamicResonator({3, 3}, exciterToUse) {}

FType Dynamic1dWave::getOutputScalar() {
    switch (outputMode) {
        case DISPLACEMENT:
            return .75e3;
        case VELOCITY:
            return 1;
    }
}

void Dynamic1dWave::computeCoefficients() {
    auto p = &parameters.derived;

    // Grid-spacing, from stability condition.
    p->h = sqrt(p->cSq * p->kSq + 4 * p->sigma1 * p->k);
    p->hSq = pow(p->h, 2);

    NFrac = L / p->h;

    double integralPart;
    alpha = modf(NFrac, &integralPart);
    p->N = static_cast<unsigned int>(integralPart);

    // Compute model coefficients.
    p->lambda = (p->k * p->c) / p->h;
    auto lambdaSq = pow(p->lambda, 2);
    auto S = (2 * p->sigma1 * p->k) / p->hSq;

    coeffs = {
            // u_l^n
            2. * (1. - lambdaSq - S),
            // u_{l±1}^n
            lambdaSq + S,
            // u_l^{n-1}
            (p->sigma0 * p->k) + (2 * S) - 1.,
            // u_{l±1}^{n-1}
            -S
    };

    p->rawCoeffs = coeffs;
    p->schemeDivisor = 1. + (p->sigma0 * p->k);

    // Scale all coefficients by the divisor to the FDS
    for (auto &coeff: coeffs) {
        coeff /= p->schemeDivisor;
    }
}

void Dynamic1dWave::computeScheme() {
    // VIRTUAL GRIDPOINTS
    // Create quadratic interpolator.
    auto am{alpha - 1.};
    auto ap{alpha + 1.};
    std::vector<FType> ip{-am / ap, 1., am / ap};

    // Current
    std::vector<FType> virtualGridpoints{
            // To be applied to end of u
            ip[2] * u[1][Mu] + ip[1] * w[1][0], // w[1][1] == 0
            // To be applied to beginning of w
            ip[0] * u[1][Mu - 1] + ip[1] * u[1][Mu] + ip[2] * w[1][0]
    };
    // Previous -- TODO: just store the previous ones? Might not work if alpha changes...
    std::vector<FType> virtualGridpointsPrev{
            ip[2] * u[2][Mu] + ip[1] * w[2][0],
            ip[0] * u[2][Mu - 1] + ip[1] * u[2][Mu] + ip[2] * w[2][0]
    };

//    std::vector<std::vector<FType>> virtualGridpoints{
//            // Current
//            {
//                    // To be applied to end of u
//                    ip[2] * u[1][Mu] + ip[1] * w[1][0], // w[1][1] == 0
//                    // To be applied to beginning of w
//                    ip[0] * u[1][Mu - 1] + ip[1] * u[1][Mu] + ip[2] * w[1][0]
//            },
//            // Previous
//            {
//                    ip[2] * u[2][Mu] + ip[1] * w[2][0],
//                    ip[0] * u[2][Mu - 1] + ip[1] * u[2][Mu] + ip[2] * w[2][0]
//            }
//    };

    unsigned int l;

    // Clamped boundary, so start from l=1
    for (l = 1; l < Mu; ++l) {
        u[0][l] = coeffs[0] * u[1][l] +
                  coeffs[1] * (u[1][l + 1] + u[1][l - 1]) +
                  coeffs[2] * u[2][l] +
                  coeffs[3] * (u[2][l + 1] + u[2][l - 1]);
    }

    // End of u
    l = Mu;
    u[0][l] = coeffs[0] * u[1][l] +
              coeffs[1] * (virtualGridpoints[0] + u[1][l - 1]) +
              coeffs[2] * u[2][l] +
              coeffs[3] * (virtualGridpointsPrev[0] + u[2][l - 1]);

    // Beginning of w
    l = 0;
    w[0][l] = coeffs[0] * w[1][l] +
              //          | this = 0 |
              coeffs[1] * (w[1][l + 1] + virtualGridpoints[1]) +
              coeffs[2] * w[2][l] +
              //          | this too |
              coeffs[3] * (w[2][l + 1] + virtualGridpointsPrev[1]);
    // w[n][1] is always 0, because clamped (Dirichlet)
}

void Dynamic1dWave::setTension(FType tension) {
    parameters.T = tension;
    computeDerivedParameters();
}

void Dynamic1dWave::setDensity(FType density) {
    parameters.rho = density;
    computeDerivedParameters();
}

void Dynamic1dWave::setRadius(FType radius) {
    parameters.r = radius;
    computeDerivedParameters();
}

void Dynamic1dWave::computeDerivedParameters() {
    auto r = parameters.r.getNext();
    if (r > 0.0) {
        auto p = &parameters.derived;
        p->A = M_PI * pow(r, 2);

        auto rhoA = parameters.rho.getNext() * p->A;
        if (rhoA > 0.0) {
            p->c = sqrt(parameters.T.getNext() / rhoA);
            p->cSq = pow(p->c, 2);

            auto sigmas = t60ToSigma(parameters.T60_0, parameters.T60_1);
            p->sigma0 = sigmas.first;
            p->sigma1 = sigmas.second;
        }
    }
}

void Dynamic1dWave::doDisplacementCorrection() {
    auto p = parameters.derived;

    auto onemAlphaSigCkp = (1. - alpha) * sigmaCkp;
    auto etaNext = w[0][0] - u[0][Mu];
    auto etaPrev = w[2][0] - u[2][Mu];

    auto a = (p.h * onemAlphaSigCkp) / (2. * (p.h * (alpha + epsilon) + p.kSq * onemAlphaSigCkp));
    auto F = a * (restoringForce * etaPrev + etaNext);
    auto correctionF = (F * p.kSq) / (p.h * (1. + p.sigma0 * p.k));

    u[0][Mu] += correctionF;
    w[0][0] -= correctionF;
}

void Dynamic1dWave::adjustGridDimensions() {
    auto p = parameters.derived;

    if (p.N != NPrev) {
        auto diff = static_cast<int>(NPrev) - static_cast<int>(p.N);
        jassert(abs(diff) == 1);

        Mu = p.N - 1;

        /**
         * Resizing the state vectors has two significant memory-related
         * ramifications:
         *
         * 1. The pointer-swap-based timestep increment means u[0] may not point
         *    to uStates[0][0];
         * 2. Resizing a vector may move it to a new location in memory.
         *
         * So, a bit of discretion required:
         */

        // Store the addresses that the elements of u are pointing at right now.
        std::map<FType *, unsigned int> addresses = {
                {u[0], 0},
                {u[1], 1},
                {u[2], 2}
        };
        // Resize each vector in uStates; make sure u and uStates stay in sync.
        for (unsigned long i = 0; i < stencilDimensions.second; ++i) {
            // Get the address of the beginning of uStates[i]
            auto currentAddress = &uStates[i][0];
            // Get the index of the corresponding u
            auto uIndex = addresses[currentAddress];
            // Resize uStates[i] -- might move uStates[i][0] to a new address
            uStates[i].resize(Mu + 1);
            // Point the appropriate u at the beginning of uStates[i]
            u[uIndex] = &uStates[i][0];
        }

        if (p.N > NPrev) {
            // Adding a gridpoint: interpolate a displacement for the new
            // gridpoint.
            std::vector<FType> cubicIp{
                    (-alpha * (alpha + 1.)) / ((alpha + 2.) * (alpha + 3.)),
//                    (2. * alpha) / (alpha + 2.), // u[n][Mu] is always zero at this point.
                    2. / (alpha + 2.),
//                    2 * alpha / -((alpha + 3) * (alpha + 2))
            };

            u[1][Mu] = cubicIp[0] * u[1][Mu - 1] +
                       cubicIp[1] * w[1][0];

            u[2][Mu] = cubicIp[0] * u[2][Mu - 1] +
                       cubicIp[1] * w[2][0];
        }
    }

    // Store current N to check against next iteration.
    NPrev = p.N;
}

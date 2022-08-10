//
// Created by Tommy Rushton on 09/08/2022.
//

#include "Dynamic1dWave.h"

Dynamic1dWave::Dynamic1dWave(Exciter *exciterToUse) :
        DynamicResonator(std::make_pair(3, 3), exciterToUse) {}

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
    p->N = static_cast<unsigned int>(floor(NFrac));
    NPrev = p->N;
    alpha = NFrac - p->N;

    Mu = p->N - 1;
    Mw = 1;

    // Compute model coefficients.
    p->lambda = (p->k * p->c) / p->h;
    auto lambdaSq = pow(p->lambda, 2);
    auto S = (2 * p->sigma1 * p->k) / p->hSq;

    // lambda should always equal 1 here...
    coeffs = {
            // u_l^n
            2 * (1 - lambdaSq - S),
            // u_{l±1}^n
            lambdaSq + S,
            // u_l^{n-1}
            p->sigma0 * p->k + 2 * S - 1,
            // u_{l±1}^{n-1}
            -S
    };

    p->rawCoeffs = coeffs;
    p->schemeDivisor = 1 + (p->sigma0 * p->k);

    // Scale all coefficients by the divisor to the FDS
    for (auto &coeff: coeffs) {
        coeff /= p->schemeDivisor;
    }

    // Displacement correction stuff
    // Damping coefficient for the spring force
    auto sigmaC = 1;
    auto sigmaCk = sigmaC/p->k;
    sigmaCkp = 1 + sigmaCk;
    sigmaCkm = 1 - sigmaCk;

    restoringForce = sigmaCkm / sigmaCkp;
}

void Dynamic1dWave::computeScheme() {
    auto p = parameters.derived;

    if (p.N != NPrev) {
        // Add or remove a gridpoint...
        jassertfalse;
    }

    // VIRTUAL GRIDPOINTS
    // Create quadratic interpolator.
    auto am{alpha - 1.};
    auto ap{alpha + 1};
    std::vector<FType> ip{-am / ap, 1., am / ap};

    std::vector<std::vector<FType>> virtualGridpoints{
            // Current
            {
                    // To be applied to end of u
                    ip[2] * u[1][Mu] + ip[1] * w[1][0], // w[1][1] == 0
                    // To be applied to beginning of w
                    ip[0] * u[1][Mu - 1] + ip[1] * u[1][Mu] + ip[2] * w[1][0]
            },
            // Previous -- TODO: just store the previous ones? Might not work if alpha changes...
            {
                    ip[2] * u[2][Mu] + ip[1] * w[2][0],
                    ip[0] * u[2][Mu - 1] + ip[1] * u[2][Mu] + ip[2] * w[2][0]
            }
    };

    unsigned int l;

    // Clamped boundary, so start from l=1
    for (l = 1; l < Mu; ++l) {
        u[0][l] = coeffs[0] * u[1][l] +
                  coeffs[1] * (u[1][l + 1] + u[1][l - 1]) +
                  coeffs[2] * u[2][l] +
                  coeffs[3] * (u[2][l + 1] + u[2][l - 1]);
    }

    l = Mu;
    u[0][l] = coeffs[0] * u[1][l] +
              coeffs[1] * (virtualGridpoints[0][0] + u[1][l - 1]) +
              coeffs[2] * u[2][l] +
              coeffs[3] * (virtualGridpoints[1][0] + u[2][l - 1]);

    l = 0;
    w[0][l] = coeffs[0] * w[1][l] +
              //          | this = 0 |
              coeffs[1] * (w[1][l + 1] + virtualGridpoints[0][1]) +
              coeffs[2] * w[2][l] +
              //          | this too |
              coeffs[3] * (w[2][l + 1] + virtualGridpoints[1][1]);
    // And w[n][1] is always 0, because clamped (Dirichlet)

    // DISPLACEMENT CORRECTION
    auto onemalpha = 1 - alpha;
    auto etaNext = w[0][0] - u[0][Mu];
    auto etaPrev = w[2][0] - u[2][Mu];

    auto a = (p.h * onemalpha * sigmaCkp) / 2 * (p.h * (alpha + epsilon) + p.kSq * onemalpha * sigmaCkp);
    auto F = a * (restoringForce * etaPrev + etaNext);
    auto correctionF = (F * p.kSq) / (p.h * (1 + p.sigma0 * p.k));

    u[0][Mu] += correctionF;
    w[0][0] += correctionF;
}

void Dynamic1dWave::setTension(FType tension) {
    parameters.T = tension;
    setDerivedParameters();
}

void Dynamic1dWave::setDensity(FType density) {
    parameters.rho = density;
    setDerivedParameters();
}

void Dynamic1dWave::setRadius(FType radius) {
    parameters.r = radius;
    setDerivedParameters();
}

void Dynamic1dWave::setDerivedParameters() {
    if (parameters.r > 0.0) {
        auto p = &parameters.derived;
        p->A = M_PI * pow(parameters.r, 2);

        auto rhoA = parameters.rho * p->A;
        if (rhoA > 0.0) {
            p->cSq = parameters.T / rhoA;
            p->c = sqrt(p->cSq);

            auto sigmas = t60ToSigma(parameters.T60_0, parameters.T60_1);
            p->sigma0 = sigmas.first;
            p->sigma1 = sigmas.second;
        }
    }
}

void Dynamic1dWave::doDisplacementCorrection() {
    jassertfalse;
}

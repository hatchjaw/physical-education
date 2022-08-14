//
// Created by Tommy Rushton on 11/08/2022.
//

#include "DynamicResonator2D.h"


DynamicResonator2D::DynamicResonator2D(
        std::tuple<unsigned int, unsigned int, unsigned int> stencilLMN,
        Exciter *exciterToUse
) : DynamicResonator(exciterToUse),
    stencilDimensions(std::move(stencilLMN)) {
    u.resize(std::get<2>(stencilDimensions));
//    exciter = exciterToUse;
}

void DynamicResonator2D::initialiseState() {
    auto p = parameters.derived;
    Utils::setupVectorPointers(u, uStates, std::get<2>(stencilDimensions), (p.Mxu + 1) * (p.Myu + 1));
    Utils::setupVectorPointers(v, vStates, std::get<2>(stencilDimensions), (p.Myu + 1) * (Nx - p.Mxu + 1));
    Utils::setupVectorPointers(w, wStates, std::get<2>(stencilDimensions), (p.Mxu + 1) * (Ny - p.Myu + 1));
    Utils::setupVectorPointers(z, zStates, std::get<2>(stencilDimensions), (Nx - p.Mxu + 1) * (Ny - p.Myu + 1));
}

void DynamicResonator2D::setLength(FType length) {
    Lx = length;
}

void DynamicResonator2D::setDimensions(std::pair<FType, FType> dimensions) {
    Lx = dimensions.first;
    Ly = dimensions.second;
}

void DynamicResonator2D::setOutputPositions(std::vector<std::pair<FType, FType>> outputPositions) {
    jassert(NxFrac > 0 && NyFrac > 0);

    if (outputPositions.size() != normalisedOutputPositions.size()) {
        Utils::setupVectorPointers(uOut, uOutStates, 3, outputPositions.size());
        normalisedOutputPositions.resize(outputPositions.size());
    }

    // TODO: prevent discontinuities when output positions change.
    for (unsigned long i = 0; i < outputPositions.size(); ++i) {
        setOutputPosition(i, outputPositions[i]);
    }
}

void DynamicResonator2D::setOutputPosition(unsigned long positionIndex, std::pair<FType, FType> normalisedPosition) {
    normalisedOutputPositions[positionIndex] = {
            Utils::clamp(normalisedPosition.first, 0.f, 1.f),
            Utils::clamp(normalisedPosition.second, 0.f, 1.f)
    };
}

void DynamicResonator2D::initialiseModel(FType sampleRate) {
    parameters.derived.k = 1.0 / sampleRate;
    parameters.derived.kSq = pow(parameters.derived.k, 2);
    computeDerivedParameters();
    computeCoefficients();

    // Spatial dimensions
    parameters.derived.Mxu = Nx - 1;
    parameters.derived.Myu = Ny - 1;
    NxPrev = Nx;
    NyPrev = Ny;

    exciter->setupExcitation();

    initialiseState();

    // Displacement correction stuff
    // Damping coefficient for the spring force
    auto sigmaCk = sigmaC / parameters.derived.k;
    sigmaCkp = 1. + sigmaCk;
    sigmaCkm = 1. - sigmaCk;
    restoringForce = sigmaCkm / sigmaCkp;

    isInitialised = true;
}

// TODO: DRY this out
FType DynamicResonator2D::getOutputAtPosition(unsigned long outputPositionIndex) {
    jassert(isInitialised);
    jassert(outputPositionIndex < normalisedOutputPositions.size());

    auto positionX = Utils::clamp(
            normalisedOutputPositions[outputPositionIndex].first * NxFrac,
            1.f,
            parameters.derived.Mxu - 2.f
    );
    auto positionY = Utils::clamp(
            normalisedOutputPositions[outputPositionIndex].second * NyFrac,
            1.f,
            parameters.derived.Myu - 2.f
    );

    // Separate the integer and fractional parts of the read position.
    float readPosX, readPosY;
    auto alphX = modf(positionX, &readPosX);
    auto alphY = modf(positionY, &readPosY);

    // Get the displacement, interpolated around the read position.
//    auto displacement = Utils::interpolate2D(u[0], parameters.derived.Mxu+1, {readPosX, readPosY}, {alphX, alphY});
    auto displacement = u[0][3 * (parameters.derived.Mxu+1) + 5];

    // Save it to the displacement buffer.
    uOut[0][outputPositionIndex] = displacement;

    switch (outputMode) {
        case DISPLACEMENT: {
            // Adjust amplitude wrt the output position. Positions close to the
            // centre have greater displacement than positions at the extremities.
            // TODO: maybe get rid of this, as it probably only holds for strings.
            auto positionAdjustment = 1;//2 * fabs(normalisedOutputPositions[outputPositionIndex] - .5);
            // Just return the appropriately scaled displacement.
            return getOutputScalar() * sqrt(positionAdjustment) * displacement;
        }
        case VELOCITY:
            // Return the appropriately scaled velocity, taken as the
            // difference between the current displacement and the displacement
            // two samples ago.
            return getOutputScalar() *
                   (1 / (2 * parameters.derived.k)) *
                   (displacement - uOut[2][outputPositionIndex]);
        default:
            jassertfalse;
    }
}

void DynamicResonator2D::advanceTimestep() {
    DynamicResonator::advanceTimestep();
    Utils::pointerSwap(v);
    Utils::pointerSwap(z);
}

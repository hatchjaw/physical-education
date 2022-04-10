//
// Created by Tommy Rushton on 07/04/2022.
//

#include "RaisedCosine.h"
#include "../Utils.h"

void RaisedCosine::startExcitation(float excitationPosition,
                                   float excitationForce,
                                   float excitationVelocity) {
    Exciter::startExcitation(excitationPosition, excitationForce, excitationVelocity);

    // Keep the width sensible relative to the total number of grid-points, and
    // respect the boundary conditions.
    auto p = resonatorParameters.derived;
    auto excitationWidth = Utils::clamp(
            floor((p.N * WIDTH_SCALAR) * force),
            3,
            static_cast<float>(p.N - 3));
    width = static_cast<unsigned int>(excitationWidth);
    auto halfWidth = .5f * excitationWidth;

    // Calculate the excitation position as a proportion of N.
    auto pos = Utils::clamp(position, 0.f, 1.f);
    // Find the nearest integer start index; also ensure the excitation can't
    // exceed the bounds of the grid, and respect the boundary conditions.
    start = static_cast<unsigned int>(std::min(
            std::max(static_cast<int>(floor(static_cast<float>(p.N) * pos - halfWidth)), 2),
            static_cast<int>(p.N - width)
    ));

    durationSamples = static_cast<unsigned int>(
            std::max(static_cast<int>(round(velocity * DURATION_SCALAR)), 2)
    );
    sampleCount = durationSamples;
}

void RaisedCosine::applyExcitation(std::vector<double *> &state) {
    if (sampleCount > 0) {
        // No need to repeatedly calculate force/2 in the loop, so do it here.
        // Also use a raised cosine to distribute the excitation over time.
        auto forceToUse = force * FORCE_SCALAR * .5 *
                          (1 - cos((M_PI * sampleCount) / (durationSamples * .5)));
        auto halfWidth = .5f * static_cast<float>(width);

        // Apply the excitation by adding displacement to the identified range of
        // grid-points. Negative because that corresponds with upward displacement.
        for (unsigned int l = 0; l < width; ++l) {
            state[2][l + start] -= forceToUse * (1 - cos((M_PI * l) / halfWidth));
        }

        --sampleCount;
    } else {
        isExciting = false;
    }
}

void RaisedCosine::setWidth(double normalisedWidth) {
    width = static_cast<unsigned int>(floor(resonatorParameters.derived.N * normalisedWidth));
}

void RaisedCosine::stopExcitation() {
    Exciter::stopExcitation();
//    sampleCount = 0;
}
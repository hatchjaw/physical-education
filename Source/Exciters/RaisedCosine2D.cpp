//
// Created by Tommy Rushton on 12/08/2022.
//

#include "RaisedCosine2D.h"

void RaisedCosine2D::startExcitation(std::pair<float, float> excitationPosition,
                                     float excitationForce,
                                     float excitationVelocity) {
    Exciter2D::startExcitation(excitationPosition, excitationForce, excitationVelocity);
    position.x.set(excitationPosition.first, true);
    position.y.set(excitationPosition.second, true);

    // Keep the width sensible relative to the total number of grid-points, and
    // respect the boundary conditions.
    auto p = resonatorParameters.derived;
    auto excitationWidth = Utils::clamp(
            floor((p.Mxu * WIDTH_SCALAR) * force),
            3,
            static_cast<float>(std::min(p.Mxu, p.Myu) - 3));
    width = static_cast<unsigned int>(excitationWidth);
    auto halfWidth = .5f * excitationWidth;

    // Calculate the excitation position as a proportion of N.
    auto posX = Utils::clamp(position.x.getNext(), 0.f, 1.f);
    auto posY = Utils::clamp(position.y.getNext(), 0.f, 1.f);
    // Find the nearest integer start index; also ensure the excitation can't
    // exceed the bounds of the grid, and respect the boundary conditions.
    start.l = static_cast<unsigned int>(std::min(
            std::max(static_cast<int>(floor(static_cast<float>(p.Mxu) * posX - halfWidth)), 2),
            static_cast<int>(p.Mxu - width)
    ));
    start.m = static_cast<unsigned int>(std::min(
            std::max(static_cast<int>(floor(static_cast<float>(p.Myu) * posY - halfWidth)), 2),
            static_cast<int>(p.Myu - width)
    ));

    durationSamples = static_cast<unsigned int>(
            std::max(static_cast<int>(round(velocity * DURATION_SCALAR)), 2)
    );
    sampleCount = durationSamples;
}

void RaisedCosine2D::applyExcitation(std::vector<FType *> &state) {
    if (sampleCount > 0) {
        auto p = resonatorParameters.derived;
        // No need to repeatedly calculate force/2 in the loop, so do it here.
        // Also use a raised cosine to distribute the excitation over time.
        auto forceToUse = force * FORCE_SCALAR * .5 *
                          (1 - cos((M_PI * sampleCount) / (durationSamples * .5)));
        auto halfWidth = .5f * static_cast<float>(width);

        // Number of gridpoints in the l spatial domain.
        auto lLength = p.Mxu + 1;

        // Apply the excitation by adding displacement to the identified range of
        // grid-points.
        for (unsigned int m = 0; m < width; ++m) {
            // Raised cosine for the y dimension...
            auto cos2D = 1 - cos((M_PI * m) / halfWidth);
            auto rowStart = lLength * (m + start.m) + start.l;
            for (unsigned int l = 0; l < width; ++l) {
                auto lm = rowStart + l;
                // ...multiplied by a raised cosine for the x dimension, *and*
                // the temporal raised cosine calculated above.
                state[0][lm] -= forceToUse * cos2D * (1 - cos((M_PI * l) / halfWidth));
            }
        }

        --sampleCount;
    } else {
        isExciting = false;
    }
}

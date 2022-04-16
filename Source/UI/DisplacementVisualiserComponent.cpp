//
// Created by Tommy Rushton on 25/03/2022.
//

#include "DisplacementVisualiserComponent.h"

DisplacementVisualiserComponent::DisplacementVisualiserComponent(std::vector<FType> &modelState) :
        displacement(modelState) {
    this->startTimerHz(15);
}

DisplacementVisualiserComponent::~DisplacementVisualiserComponent() = default;

void DisplacementVisualiserComponent::paint(juce::Graphics &g) {
    // Set the colour of the path
    g.setColour(Colours::lightskyblue);

    // Generate the path that visualises the state of the system.
    auto visualStatePath = generateStatePath();

    // Draw the path using a stroke (thickness) of 2 pixels.
    g.strokePath(visualStatePath, PathStrokeType(2.0f));
}

void DisplacementVisualiserComponent::resized() {}

juce::Path DisplacementVisualiserComponent::generateStatePath() {
    // Make a copy of the displacement vector for this iteration, otherwise the
    // values will probably be overwritten while the path is being constructed
    // and visual discontinuities will result.
    std::vector<FType> u(this->displacement.size());
    std::copy(this->displacement.begin(), this->displacement.end(), u.begin());

    auto height = static_cast<float>(getHeight());

    // we have to scale up the state of the system from 'transverse
    // displacement' to 'pixels'
    auto visualScaling = 200.f * height;

    // String-boundaries are in the vertical middle of the component
    auto stringBoundaries = height / 2.f;

    // Initialise path
    juce::Path stringPath;

    // Start path
    stringPath.startNewSubPath(0, static_cast<float>(-u[0]) * visualScaling +
                                  stringBoundaries);
    auto N = u.size();

    // Visual spacing between grid points
    auto spacing = static_cast<float>(this->getWidth()) / static_cast<float>(N - 1);
    auto x = spacing;

    for (unsigned long l = 1; l < N; l++) {
        // Displacement needs to be negative, because a positive displacement
        // would visually go down.
        auto newY = static_cast<float>(-u[l]) * visualScaling + stringBoundaries;

        // Prevent NaN values throwing an exception.
        if (isnan(newY)) {
            newY = 0;
        }

        stringPath.lineTo(x, newY);
        x += spacing;
    }

    return stringPath;
}

void DisplacementVisualiserComponent::timerCallback() {
    this->repaint();
}
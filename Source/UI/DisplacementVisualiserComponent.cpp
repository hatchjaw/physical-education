//
// Created by Tommy Rushton on 25/03/2022.
//

#include "DisplacementVisualiserComponent.h"

DisplacementVisualiserComponent::DisplacementVisualiserComponent(std::vector<double> &modelState) :
        displacement(modelState) {
    this->startTimerHz(15);
}

DisplacementVisualiserComponent::~DisplacementVisualiserComponent() = default;

void DisplacementVisualiserComponent::paint(juce::Graphics &g) {
    // Set the colour of the path
    g.setColour(Colours::cyan);

    // Generate the path that visualises the state of the system.
    auto visualStatePath = visualiseState(g);

    // Draw the path using a stroke (thickness) of 2 pixels.
    g.strokePath(visualStatePath, PathStrokeType(2.0f));
}

void DisplacementVisualiserComponent::resized() {}

juce::Path DisplacementVisualiserComponent::visualiseState(juce::Graphics &g) {
    // Make a copy of the displacement vector for this iteration, otherwise the
    // values will probably be overwritten while the path is being constructed
    // and visual discontinuities will result. (NB, might not actually work.)
    auto u = this->displacement;

    // we have to scale up the state of the system from 'transverse
    // displacement' to 'pixels'
    auto visualScaling = 200.f;

    // String-boundaries are in the vertical middle of the component
    auto stringBoundaries = static_cast<float>(this->getHeight()) / 2.f;

    // Initialise path
    juce::Path stringPath;

    // Start path
    stringPath.startNewSubPath(0, static_cast<float>(-u[0]) * visualScaling +
                                  stringBoundaries);
    auto N = this->displacement.size();

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
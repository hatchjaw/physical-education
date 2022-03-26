//
// Created by Tommy Rushton on 25/03/2022.
//

#include "DisplacementVisualiserComponent.h"

DisplacementVisualiserComponent::DisplacementVisualiserComponent(double *&modelState) :
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
    // we have to scale up the state of the system from 'transverse
    // displacement' to 'pixels'
    auto visualScaling = 200.f;

    // String-boundaries are in the vertical middle of the component
    auto stringBoundaries = static_cast<float>(this->getHeight()) / 2.f;

    // Initialise path
    juce::Path stringPath;

    // Start path
    stringPath.startNewSubPath(0, static_cast<float>(-this->displacement[0]) * visualScaling +
                                                     stringBoundaries);
    auto N = 50;

    // Visual spacing between two grid points
    auto spacing = static_cast<float>(this->getWidth()) / static_cast<float>(N);
    auto x = spacing;

    for (int l = 1; l <= N; l++) {
        // Needs to be -u, because a positive u would visually go down
        auto newY = static_cast<float>(-this->displacement[l]) * visualScaling + stringBoundaries;

        // if we get NAN values, make sure that we don't get an exception
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
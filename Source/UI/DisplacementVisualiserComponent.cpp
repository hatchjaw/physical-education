//
// Created by Tommy Rushton on 25/03/2022.
//

#include "DisplacementVisualiserComponent.h"
#include "../Exciters/Bow.h"
#include "../Exciters/RaisedCosine.h"
#include "../Resonators/StiffString.h"
#include "../Resonators/Dynamic1dWave.h"

DisplacementVisualiserComponent::DisplacementVisualiserComponent(Resonator *&r) :
        resonator(r) {
    this->startTimerHz(15);
}

DisplacementVisualiserComponent::~DisplacementVisualiserComponent() = default;

void DisplacementVisualiserComponent::paint(juce::Graphics &g) {
    auto width = static_cast<float>(getWidth());
    auto height = static_cast<float>(getHeight());

    // Set the colour of the path
    g.setColour(Colours::lightskyblue);

    if (auto stiffString = dynamic_cast<StiffString *>(resonator)) {
        // Generate the path that visualises the state of the system.
        auto visualStatePath = generateStatePath();

        // Draw the path using a stroke (thickness) of 2 pixels.
        g.strokePath(visualStatePath, PathStrokeType(2.0f));

        // Draw spring-damper
        auto damperPos = static_cast<float>(resonator->getDamperPosition());
        g.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, .25f));
        g.fillEllipse(
                PADDING_HORIZONTAL + damperPos * (width - 2.f * PADDING_HORIZONTAL) - DAMPER_INDICATOR_DIAMETER * .5f,
                (height - DAMPER_INDICATOR_DIAMETER) * .5f,
                DAMPER_INDICATOR_DIAMETER,
                DAMPER_INDICATOR_DIAMETER);
    } else if (auto dynamic1dWave = dynamic_cast<Dynamic1dWave *>(resonator)) {
        gridSpacing = static_cast<float>(resonator->getGridSpacing() / resonator->getLength());
        // This isn't pretty, but it's one way of avoiding drawing an out-of-
        // date w, thus giving the appearance that displacement correction isn't
        // working.
        auto state = dynamic1dWave->getState();
        std::vector<FType> u(state.size());
        std::copy(state.begin(), state.end(), u.begin());
        state = dynamic1dWave->getStaticState();
        std::vector<FType> w(state.size());
        std::copy(state.begin(), state.end(), w.begin());

        auto path = generateStatePathDynamic(u);
        g.strokePath(path, PathStrokeType(2.0f));
        path = generateStatePathStatic(w);
        g.strokePath(path, PathStrokeType(2.0f));
    }

    // Draw excitation
    auto excitationPos = static_cast<float>(resonator->getExcitationPosition());
    auto exciter = resonator->getExciter();
    auto velocity = exciter->getVelocity();
    auto force = exciter->getForce();
    g.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, force));
    if (auto bow = dynamic_cast<Bow *>(exciter)) {
        g.fillRect(
                PADDING_HORIZONTAL + excitationPos * (width - 2.f * PADDING_HORIZONTAL) - BOW_INDICATOR_WIDTH * .5f,
                (height - 2 * BOW_INDICATOR_HEIGHT * velocity) * .5f,
                BOW_INDICATOR_WIDTH,
                BOW_INDICATOR_HEIGHT
        );
    } else if (auto cos = dynamic_cast<RaisedCosine *>(exciter)) {
        g.fillEllipse(
                PADDING_HORIZONTAL + excitationPos * (width - 2.f * PADDING_HORIZONTAL) -
                DAMPER_INDICATOR_DIAMETER * .5f,
                (height - DAMPER_INDICATOR_DIAMETER) * .5f,
                DAMPER_INDICATOR_DIAMETER,
                DAMPER_INDICATOR_DIAMETER
        );
    }
}

void DisplacementVisualiserComponent::resized() {}

juce::Path DisplacementVisualiserComponent::generateStatePath() {
    // Make a copy of the displacement vector for this iteration, otherwise the
    // values will probably be overwritten while the path is being constructed
    // and visual discontinuities will result.
    auto state = resonator->getState();
    std::vector<FType> u(state.size());
    std::copy(state.begin(), state.end(), u.begin());

    auto height = static_cast<float>(getHeight());

    // we have to scale up the state of the system from 'transverse
    // displacement' to 'pixels'
    auto verticalScaling = scalingFactor * height;

    // String-boundaries are in the vertical middle of the component
    auto stringEquilibrium = height * .5f;

    // Initialise path
    juce::Path stringPath;

    // Start path
    stringPath.startNewSubPath(PADDING_HORIZONTAL, static_cast<float>(-u[0]) * verticalScaling +
                                                   stringEquilibrium);
    auto N = u.size();

    // Visual spacing between grid points
    auto spacing = (static_cast<float>(this->getWidth()) - PADDING_HORIZONTAL * 2.f) / static_cast<float>(N - 1);
    auto x = PADDING_HORIZONTAL + spacing;

    for (unsigned long l = 1; l < N; l++) {
        // Displacement needs to be negative, because a positive displacement
        // would visually go down.
        auto newY = static_cast<float>(-u[l]) * verticalScaling + stringEquilibrium;

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

juce::Path DisplacementVisualiserComponent::generateStatePathDynamic(std::vector<FType> &state) {
    juce::Path path;

    auto height = static_cast<float>(getHeight());

    // Scale state of the system from 'transverse displacement' to pixels
    auto verticalScaling = scalingFactor * height;

    // String-boundaries are in the vertical middle of the component
    auto stringEquilibrium = height * .5f;

    path.startNewSubPath(PADDING_HORIZONTAL, static_cast<float>(-state[0]) * verticalScaling +
                                             stringEquilibrium);
    auto N = state.size();

    // Visual spacing between grid points
    auto spacing = (static_cast<float>(this->getWidth()) - PADDING_HORIZONTAL * 2.f) * gridSpacing;
    auto x = PADDING_HORIZONTAL + spacing;

    for (unsigned long l = 1; l < N; l++) {
        // Displacement needs to be negative, because a positive displacement
        // would visually go down.
        auto newY = static_cast<float>(-state[l]) * verticalScaling + stringEquilibrium;

        // Prevent NaN values throwing an exception.
        if (isnan(newY)) {
            newY = 0;
        }

        path.lineTo(x, newY);
        x += spacing;
    }

    return path;
}

juce::Path DisplacementVisualiserComponent::generateStatePathStatic(std::vector<FType> &state) {
    juce::Path path;

    auto height = static_cast<float>(getHeight());
    auto width = static_cast<float>(getWidth());

    // Scale state of the system from 'transverse displacement' to pixels
    auto verticalScaling = scalingFactor * height;

    // String-boundaries are in the vertical middle of the component
    auto stringEquilibrium = height * .5f;

    auto l = state.size() - 1;

    path.startNewSubPath(
            width - PADDING_HORIZONTAL,
            static_cast<float>(-state[l]) * verticalScaling + stringEquilibrium
    );

    auto spacing = (static_cast<float>(this->getWidth()) - PADDING_HORIZONTAL * 2.f) * gridSpacing;
    auto x = width - PADDING_HORIZONTAL - spacing;

    while (l-- > 0) {
        auto newY = static_cast<float>(-state[l]) * verticalScaling + stringEquilibrium;

        // Prevent NaN values throwing an exception.
        if (isnan(newY)) {
            newY = 0;
        }

        path.lineTo(x, newY);

        x -= spacing;
    }

    return path;
}

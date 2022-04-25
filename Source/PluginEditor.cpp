//
// Created by Tommy Rushton on 25/03/2022.
//

#include "PluginEditor.h"
#include "Constants.h"

PhysicalEducationAudioProcessorEditor::PhysicalEducationAudioProcessorEditor(
        PhysicalEducationAudioProcessor &p
) : AudioProcessorEditor(&p),
    audioProcessor(p),
    displacementVisualiserComponent(audioProcessor.getModelState()),
    outputModeComponent(audioProcessor.apvts, Constants::ParameterIDs::OUTPUT_MODE),
    outputPositionsComponent(audioProcessor.apvts, Constants::ParameterIDs::OUT_POS_1,
                             Constants::ParameterIDs::OUT_POS_2),
    excitationTypeComponent(audioProcessor.apvts, Constants::ParameterIDs::EXCITATION_TYPE),
    collisionParamsComponent(audioProcessor.apvts,
                             Constants::ParameterIDs::COLLISION_POS,
                             Constants::ParameterIDs::COLLISION_STIFFNESS,
                             Constants::ParameterIDs::COLLISION_OMEGA1,
                             Constants::ParameterIDs::COLLISION_DAMPING) {

    setSize(800, 600);

    // This has to be first, so everything else is drawn on top of it.
    addAndMakeVisible(displacementVisualiserComponent);

//    addAndMakeVisible(resetButton);
//    resetButton.setButtonText("Reset");
//    resetButton.onClick = [this] { audioProcessor.resetVoices(); };

    addAndMakeVisible(outputPositionsComponent);
    // Got to add this after output position component else it can't be clicked.
    addAndMakeVisible(outputModeComponent);
    addAndMakeVisible(excitationTypeComponent);
    addAndMakeVisible(collisionParamsComponent);
}

PhysicalEducationAudioProcessorEditor::~PhysicalEducationAudioProcessorEditor() {
}

void PhysicalEducationAudioProcessorEditor::paint(Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PhysicalEducationAudioProcessorEditor::resized() {
    auto area = getLocalBounds();
    auto width = getWidth();
    auto height = getHeight();
    auto bottom = getBottom();
    auto right = getRight();

//    resetButton.setBounds(right - 100, 0, 100, Constants::Layout::EXCITATION_TYPE_HEIGHT);

    displacementVisualiserComponent.setBounds(area);

    outputModeComponent.setBounds(
            right - Constants::Layout::OUTPUT_MODE_WIDTH,
            bottom - Constants::Layout::OUTPUT_POSITIONS_HEIGHT,
            Constants::Layout::OUTPUT_MODE_WIDTH,
            static_cast<int>(.5 * Constants::Layout::OUTPUT_POSITIONS_HEIGHT)
    );

    outputPositionsComponent.setBounds(
            0,
            bottom - Constants::Layout::OUTPUT_POSITIONS_HEIGHT,
            width,
            Constants::Layout::OUTPUT_POSITIONS_HEIGHT
    );

    excitationTypeComponent.setBounds(0, 0, Constants::Layout::EXCITATION_TYPE_WIDTH,
                                      Constants::Layout::EXCITATION_TYPE_HEIGHT);

    collisionParamsComponent.setBounds(0, excitationTypeComponent.getBottom() + 5, width,
                                       Constants::Layout::COLLISION_PARAMS_HEIGHT);
}

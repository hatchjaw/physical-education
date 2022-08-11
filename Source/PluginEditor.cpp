//
// Created by Tommy Rushton on 25/03/2022.
//

#include "PluginEditor.h"
#include "Constants.h"

PhysicalEducationAudioProcessorEditor::PhysicalEducationAudioProcessorEditor(
        PhysicalEducationAudioProcessor &p
) : AudioProcessorEditor(&p),
    audioProcessor(p),
    resonatorTypeComponent(audioProcessor.apvts, Constants::ParameterIDs::RESONATOR_TYPE),
    displacementVisualiserComponent(audioProcessor.getResonator()),
    outputModeComponent(audioProcessor.apvts, Constants::ParameterIDs::OUTPUT_MODE),
    outputPositionsComponent(audioProcessor.apvts, Constants::ParameterIDs::OUT_POS_1,
                             Constants::ParameterIDs::OUT_POS_2),
    excitationTypeComponent(audioProcessor.apvts, Constants::ParameterIDs::EXCITATION_TYPE),
    damperParamsComponent(audioProcessor.apvts,
                          Constants::ParameterIDs::DAMPER_POS,
                          Constants::ParameterIDs::DAMPER_STIFFNESS,
                          Constants::ParameterIDs::DAMPER_NONLINEARITY,
                          Constants::ParameterIDs::DAMPER_LOSS),
    wave1dParamsComponent(audioProcessor.apvts,
                          Constants::ParameterIDs::TENSION) {

    setSize(800, 600);

    // This has to be first, so everything else is drawn on top of it.
    addAndMakeVisible(displacementVisualiserComponent);

//    addAndMakeVisible(resetButton);
//    resetButton.setButtonText("Reset");
//    resetButton.onClick = [this] { audioProcessor.resetVoices(); };

    addAndMakeVisible(outputPositionsComponent);
    // Got to add this after output position component else it can't be clicked.
    addAndMakeVisible(outputModeComponent);

    addAndMakeVisible(resonatorTypeComponent);
    resonatorTypeComponent.onChange = [this](int value) {
        auto resType = Constants::RESONATOR_TYPES[value];
        damperParamsComponent.setVisible(resType == "Stiff String");
        wave1dParamsComponent.setVisible(resType == "Dynamic 1D Wave");
    };

    addAndMakeVisible(excitationTypeComponent);

    addChildComponent(damperParamsComponent);
    addChildComponent(wave1dParamsComponent);
}

PhysicalEducationAudioProcessorEditor::~PhysicalEducationAudioProcessorEditor() {
}

void PhysicalEducationAudioProcessorEditor::paint(Graphics &g) {
    g.fillAll(juce::Colour{49, 73, 83});
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

    resonatorTypeComponent.setBounds(0, 0,
                                     Constants::Layout::EXCITATION_TYPE_WIDTH,
                                     Constants::Layout::EXCITATION_TYPE_HEIGHT);

    excitationTypeComponent.setBounds(resonatorTypeComponent.getRight() + 10, 0,
                                      Constants::Layout::EXCITATION_TYPE_WIDTH,
                                      Constants::Layout::EXCITATION_TYPE_HEIGHT);

    damperParamsComponent.setBounds(0, excitationTypeComponent.getBottom() + 5,
                                    width,
                                    Constants::Layout::DAMPER_PARAMS_HEIGHT);

    wave1dParamsComponent.setBounds(damperParamsComponent.getX(), damperParamsComponent.getY(),
                                    damperParamsComponent.getWidth(), damperParamsComponent.getHeight());
}

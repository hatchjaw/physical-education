//
// Created by Tommy Rushton on 25/03/2022.
//

#include "PluginEditor.h"
#include "Layout.h"

PhysicalEducationAudioProcessorEditor::PhysicalEducationAudioProcessorEditor(
        PhysicalEducationAudioProcessor &p
) : AudioProcessorEditor(&p),
    audioProcessor(p),
    displacementVisualiserComponent(audioProcessor.getModelState()),
    outputModeComponent(audioProcessor.apvts, "OUTPUT_MODE"),
    outputPositionsComponent(audioProcessor.apvts, "OUT_POS_1", "OUT_POS_2") {

    setSize(800, 600);

    addAndMakeVisible(displacementVisualiserComponent);
    addAndMakeVisible(outputPositionsComponent);
    // Got to add this after output position component else it can't be clicked.
    addAndMakeVisible(outputModeComponent);
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

    displacementVisualiserComponent.setBounds(area);

    outputModeComponent.setBounds(
            right - Layout::OUTPUT_MODE_WIDTH,
            bottom - Layout::OUTPUT_POSITIONS_HEIGHT,
            Layout::OUTPUT_MODE_WIDTH,
            static_cast<int>(.5 * Layout::OUTPUT_POSITIONS_HEIGHT)
    );

    outputPositionsComponent.setBounds(
            0,
            bottom - Layout::OUTPUT_POSITIONS_HEIGHT,
            width,
            Layout::OUTPUT_POSITIONS_HEIGHT
    );
}

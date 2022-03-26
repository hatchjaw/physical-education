//
// Created by Tommy Rushton on 25/03/2022.
//

#include "PluginEditor.h"

PhysicalEducationAudioProcessorEditor::PhysicalEducationAudioProcessorEditor(
        PhysicalEducationAudioProcessor &p
) : AudioProcessorEditor(&p),
    audioProcessor(p),
    displacementVisualiserComponent(audioProcessor.getModelState()) {
    this->setSize(800, 600);

    addAndMakeVisible(displacementVisualiserComponent);
}

PhysicalEducationAudioProcessorEditor::~PhysicalEducationAudioProcessorEditor() {

}

void PhysicalEducationAudioProcessorEditor::paint(Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PhysicalEducationAudioProcessorEditor::resized() {
    auto area = getLocalBounds();

    this->displacementVisualiserComponent.setBounds(area);
}

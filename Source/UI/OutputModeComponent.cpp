//
// Created by Tommy Rushton on 16/04/2022.
//

#include "OutputModeComponent.h"

OutputModeComponent::OutputModeComponent(AudioProcessorValueTreeState &apvts,
                                         const String &outputModeID) {
    auto choices = dynamic_cast<juce::AudioParameterChoice *>(
            apvts.getParameter(outputModeID)
    )->choices;
    outputModeSelector.addItemList(choices, 1);
    addAndMakeVisible(outputModeSelector);

    outputModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts,
            outputModeID,
            outputModeSelector
    );

    outputModeLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    outputModeLabel.setJustificationType(juce::Justification::left);
    outputModeLabel.setFont(15.f);
    addAndMakeVisible(outputModeLabel);
}

OutputModeComponent::~OutputModeComponent() {
}

void OutputModeComponent::paint(Graphics &g) {
//    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
//
//    g.setColour(juce::Colours::grey);
//    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
}

void OutputModeComponent::resized() {
    outputModeLabel.setBounds(0, 0, getWidth() * .4, getHeight());
    outputModeSelector.setBounds(outputModeLabel.getRight(), 0, getWidth() * .6, getHeight());
}
//
// Created by Tommy Rushton on 16/04/2022.
//

#include "OutputPositionsComponent.h"

OutputPositionsComponent::OutputPositionsComponent(
        AudioProcessorValueTreeState &apvts,
        const String &outPos1ID,
        const String &outPos2ID
) {
    outputPositionsSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    outputPositionsSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(outputPositionsSlider);

    outputPositionsAttachment = std::make_unique<TwoValueSliderAttachment>(
            apvts,
            outPos1ID,
            outPos2ID,
            outputPositionsSlider);

    outputPositionsLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    outputPositionsLabel.setJustificationType(juce::Justification::left);
    outputPositionsLabel.setFont(15.f);
    addAndMakeVisible(outputPositionsLabel);
}

OutputPositionsComponent::~OutputPositionsComponent() {

}

void OutputPositionsComponent::paint(Graphics &g) {
//    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void OutputPositionsComponent::resized() {
    auto halfHeight = static_cast<int>(getHeight() * .5);
    outputPositionsLabel.setBounds(0, 0, getWidth(), halfHeight);
    outputPositionsSlider.setBounds(0, halfHeight, getWidth(), halfHeight);
}

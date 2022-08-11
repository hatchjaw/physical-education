//
// Created by Tommy Rushton on 10/08/2022.
//

#include "Wave1dParamsComponent.h"

Wave1dParamsComponent::Wave1dParamsComponent(AudioProcessorValueTreeState &apvts, const String &tensionID) {
    tensionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(tensionSlider);
    tensionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            tensionID,
            tensionSlider);
    tensionLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    tensionLabel.setJustificationType(juce::Justification::centred);
    tensionLabel.setFont(15.f);
    addAndMakeVisible(tensionLabel);
}

Wave1dParamsComponent::~Wave1dParamsComponent() {

}

void Wave1dParamsComponent::paint(juce::Graphics &g) {}

void Wave1dParamsComponent::resized() {
    auto y = 0;
    auto w = getWidth() / 7;
    auto twiceW = w * 2;
    auto h = static_cast<int>(getHeight() * .5);
    auto halfH = h / 2;

    tensionLabel.setBounds(0, 0, twiceW, halfH);
    tensionSlider.setBounds(0, tensionLabel.getBottom(), twiceW, halfH);
}
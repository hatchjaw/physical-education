//
// Created by Tommy Rushton on 13/08/2022.
//

#include "Wave2dParamsComponent.h"

Wave2dParamsComponent::Wave2dParamsComponent(AudioProcessorValueTreeState &apvts, const String &tensionID) {
    tensionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    tensionSlider.setTextValueSuffix(" N/m");
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

Wave2dParamsComponent::~Wave2dParamsComponent() {

}

void Wave2dParamsComponent::paint(juce::Graphics &g) {}

void Wave2dParamsComponent::resized() {
    auto y = 0;
    auto w = getWidth() / 7;
    auto twiceW = w * 2;
    auto h = static_cast<int>(getHeight() * .5);
    auto halfH = h / 2;

    tensionLabel.setBounds(0, y, twiceW, halfH);
    tensionSlider.setBounds(0, tensionLabel.getBottom(), twiceW, halfH);
}
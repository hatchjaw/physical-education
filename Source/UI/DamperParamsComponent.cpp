//
// Created by Tommy Rushton on 24/04/2022.
//

#include "DamperParamsComponent.h"

DamperParamsComponent::DamperParamsComponent(
        juce::AudioProcessorValueTreeState &apvts,
        const juce::String &damperPosID,
        const juce::String &damperStiffnessID,
        const juce::String &damperNonlinearityID,
        const juce::String &damperLossID
) {
    damperLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    damperLabel.setJustificationType(juce::Justification::centredLeft);
    damperLabel.setFont(15.f);
    addAndMakeVisible(damperLabel);

    damperPositionSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(damperPositionSlider);
    damperPositionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            damperPosID,
            damperPositionSlider);
    damperPositionLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    damperPositionLabel.setJustificationType(juce::Justification::left);
    damperPositionLabel.setFont(15.f);
    addAndMakeVisible(damperPositionLabel);

    //
    damperStiffnessSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(damperStiffnessSlider);
    damperStiffnessAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            damperStiffnessID,
            damperStiffnessSlider);
    damperStiffnessLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    damperStiffnessLabel.setJustificationType(juce::Justification::centred);
    damperStiffnessLabel.setFont(15.f);
    addAndMakeVisible(damperStiffnessLabel);

    //
    damperNonlinearitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(damperNonlinearitySlider);
    damperNonlinearityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            damperNonlinearityID,
            damperNonlinearitySlider);
    damperNonlinearityLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    damperNonlinearityLabel.setJustificationType(juce::Justification::centred);
    damperNonlinearityLabel.setFont(15.f);
    addAndMakeVisible(damperNonlinearityLabel);

    //
    damperDampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(damperDampingSlider);
    damperDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            damperLossID,
            damperDampingSlider);
    damperDampingLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    damperDampingLabel.setJustificationType(juce::Justification::centred);
    damperDampingLabel.setFont(15.f);
    addAndMakeVisible(damperDampingLabel);
}

DamperParamsComponent::~DamperParamsComponent() {

}

void DamperParamsComponent::paint(Graphics &g) {
//    g.setColour(juce::Colours::grey);
//    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
}

void DamperParamsComponent::resized() {
    auto y = 0;
    auto w = getWidth() / 7;
    auto twiceW = w * 2;
    auto h = static_cast<int>(getHeight() * .5);
    auto halfH = h / 2;

    damperLabel.setBounds(0, 0, w, h);
    damperStiffnessLabel.setBounds(damperLabel.getRight(), 0, twiceW, halfH);
    damperStiffnessSlider.setBounds(damperLabel.getRight(), damperStiffnessLabel.getBottom(), twiceW, halfH);
    damperNonlinearityLabel.setBounds(damperStiffnessSlider.getRight(), 0, twiceW, halfH);
    damperNonlinearitySlider.setBounds(damperStiffnessSlider.getRight(), damperNonlinearityLabel.getBottom(), twiceW,
                                       halfH);
    damperDampingLabel.setBounds(damperNonlinearitySlider.getRight(), 0, twiceW, halfH);
    damperDampingSlider.setBounds(damperNonlinearitySlider.getRight(), damperDampingLabel.getBottom(), twiceW,
                                  halfH);

    y += h;
    h = static_cast<int>(getHeight() * .25);
    damperPositionLabel.setBounds(0, y, getWidth(), h);
    damperPositionSlider.setBounds(0, y + h, getWidth(), h);
}

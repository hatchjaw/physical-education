//
// Created by Tommy Rushton on 24/04/2022.
//

#include "CollisionParamsComponent.h"

CollisionParamsComponent::CollisionParamsComponent(
        juce::AudioProcessorValueTreeState &apvts,
        const juce::String &collisionPosID,
        const juce::String &collisionStiffnessID,
        const juce::String &collisionOmega1ID,
        const juce::String &collisionDampingID
) {
    collisionPositionLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    collisionPositionLabel.setJustificationType(juce::Justification::centredLeft);
    collisionPositionLabel.setFont(15.f);
    addAndMakeVisible(collisionLabel);

    collisionPositionSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(collisionPositionSlider);
    collisionPositionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            collisionPosID,
            collisionPositionSlider);
    collisionPositionLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    collisionPositionLabel.setJustificationType(juce::Justification::left);
    collisionPositionLabel.setFont(15.f);
    addAndMakeVisible(collisionPositionLabel);

    //
//    collisionStiffnessSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    collisionStiffnessSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(collisionStiffnessSlider);
    collisionStiffnessAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            collisionStiffnessID,
            collisionStiffnessSlider);
    collisionStiffnessLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    collisionStiffnessLabel.setJustificationType(juce::Justification::centred);
    collisionStiffnessLabel.setFont(15.f);
    addAndMakeVisible(collisionStiffnessLabel);

    //
//    collisionOmega1Slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    collisionOmega1Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(collisionOmega1Slider);
    collisionOmega1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            collisionOmega1ID,
            collisionOmega1Slider);
    collisionOmega1Label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    collisionOmega1Label.setJustificationType(juce::Justification::centred);
    collisionOmega1Label.setFont(15.f);
    addAndMakeVisible(collisionOmega1Label);

    //
//    collisionDampingSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    collisionDampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(collisionDampingSlider);
    collisionDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            collisionDampingID,
            collisionDampingSlider);
    collisionDampingLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    collisionDampingLabel.setJustificationType(juce::Justification::centred);
    collisionDampingLabel.setFont(15.f);
    addAndMakeVisible(collisionDampingLabel);
}

CollisionParamsComponent::~CollisionParamsComponent() {

}

void CollisionParamsComponent::paint(Graphics &g) {
//    g.setColour(juce::Colours::grey);
//    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
}

void CollisionParamsComponent::resized() {
    auto y = 0;
    auto w = getWidth() / 7;
    auto twiceW = w * 2;
    auto h = static_cast<int>(getHeight() * .5);
    auto halfH = h / 2;

    collisionLabel.setBounds(0, 0, w, h);
    collisionStiffnessLabel.setBounds(collisionLabel.getRight(), 0, twiceW, halfH);
    collisionStiffnessSlider.setBounds(collisionLabel.getRight(), collisionStiffnessLabel.getBottom(), twiceW, halfH);
    collisionOmega1Label.setBounds(collisionStiffnessSlider.getRight(), 0, twiceW, halfH);
    collisionOmega1Slider.setBounds(collisionStiffnessSlider.getRight(), collisionOmega1Label.getBottom(), twiceW,
                                    halfH);
    collisionDampingLabel.setBounds(collisionOmega1Slider.getRight(), 0, twiceW, halfH);
    collisionDampingSlider.setBounds(collisionOmega1Slider.getRight(), collisionDampingLabel.getBottom(), twiceW,
                                     halfH);

    y += h;
    h = static_cast<int>(getHeight() * .25);
    collisionPositionLabel.setBounds(0, y, getWidth(), h);
    collisionPositionSlider.setBounds(0, y + h, getWidth(), h);
}

//
// Created by Tommy Rushton on 09/08/2022.
//

#include "ResonatorTypeComponent.h"

ResonatorTypeComponent::ResonatorTypeComponent(AudioProcessorValueTreeState &apvts,
                                               const String &resonatorTypeParamID) {
    auto choices = dynamic_cast<juce::AudioParameterChoice *>(
            apvts.getParameter(resonatorTypeParamID)
    )->choices;
    resonatorTypeSelector.addItemList(choices, 1);
    resonatorTypeSelector.onChange = [this] { triggerAsyncUpdate(); };
    addAndMakeVisible(resonatorTypeSelector);

    resonatorTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts,
            resonatorTypeParamID,
            resonatorTypeSelector
    );

    resonatorTypeLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    resonatorTypeLabel.setJustificationType(juce::Justification::left);
    resonatorTypeLabel.setFont(15.f);
    addAndMakeVisible(resonatorTypeLabel);
}

ResonatorTypeComponent::~ResonatorTypeComponent() {}

void ResonatorTypeComponent::paint(juce::Graphics &g) {}

void ResonatorTypeComponent::resized() {
    resonatorTypeLabel.setBounds(0, 0, static_cast<int>(getWidth() * .3), getHeight());
    resonatorTypeSelector.setBounds(resonatorTypeLabel.getRight(), 0, static_cast<int>(getWidth() * .7),
                                    getHeight());
}

void ResonatorTypeComponent::handleAsyncUpdate() {
    if (onChange != nullptr) {
        onChange(resonatorTypeSelector.getSelectedItemIndex());
    }
}

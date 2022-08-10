//
// Created by Tommy Rushton on 17/04/2022.
//

#include "ExcitationTypeComponent.h"

ExcitationTypeComponent::ExcitationTypeComponent(AudioProcessorValueTreeState &apvts,
                                                 const String &excitationTypeParamID) {
    auto choices = dynamic_cast<juce::AudioParameterChoice *>(
            apvts.getParameter(excitationTypeParamID)
    )->choices;
    excitationTypeSelector.addItemList(choices, 1);
    addAndMakeVisible(excitationTypeSelector);

    excitationTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts,
            excitationTypeParamID,
            excitationTypeSelector
    );

    excitationTypeLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    excitationTypeLabel.setJustificationType(juce::Justification::left);
    excitationTypeLabel.setFont(15.f);
    addAndMakeVisible(excitationTypeLabel);
}

ExcitationTypeComponent::~ExcitationTypeComponent() {}

void ExcitationTypeComponent::paint(juce::Graphics &g) {}

void ExcitationTypeComponent::resized() {
    excitationTypeLabel.setBounds(0, 0, static_cast<int>(getWidth() * .2), getHeight());
    excitationTypeSelector.setBounds(excitationTypeLabel.getRight(), 0, static_cast<int>(getWidth() * .7),
                                     getHeight());
}
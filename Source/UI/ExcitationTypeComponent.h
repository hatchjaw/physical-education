//
// Created by Tommy Rushton on 17/04/2022.
//

#ifndef PHYSICAL_EDUCATION_EXCITATIONTYPECOMPONENT_H
#define PHYSICAL_EDUCATION_EXCITATIONTYPECOMPONENT_H

#include <JuceHeader.h>

class ExcitationTypeComponent : public juce::Component {
public:
    ExcitationTypeComponent(juce::AudioProcessorValueTreeState &, const juce::String &);

    ~ExcitationTypeComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

private:
    juce::ComboBox excitationTypeSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> excitationTypeAttachment;
    juce::Label excitationTypeLabel{"Excitation Type", "Exciter"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExcitationTypeComponent)
};


#endif //PHYSICAL_EDUCATION_EXCITATIONTYPECOMPONENT_H

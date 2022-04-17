//
// Created by Tommy Rushton on 16/04/2022.
//

#ifndef PHYSICAL_EDUCATION_OUTPUTMODECOMPONENT_H
#define PHYSICAL_EDUCATION_OUTPUTMODECOMPONENT_H

#include <JuceHeader.h>

class OutputModeComponent : public juce::Component {
public:
    OutputModeComponent(juce::AudioProcessorValueTreeState &, const juce::String &);

    ~OutputModeComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

private:
    juce::ComboBox outputModeSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> outputModeAttachment;
    juce::Label outputModeLabel{"Output Mode", "Output mode"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputModeComponent)
};


#endif //PHYSICAL_EDUCATION_OUTPUTMODECOMPONENT_H

//
// Created by Tommy Rushton on 09/08/2022.
//

#ifndef PHYSICAL_EDUCATION_RESONATORTYPECOMPONENT_H
#define PHYSICAL_EDUCATION_RESONATORTYPECOMPONENT_H

#include <JuceHeader.h>

class ResonatorTypeComponent : public juce::Component,
                               public juce::AsyncUpdater {
public:
    ResonatorTypeComponent(juce::AudioProcessorValueTreeState &, const juce::String &);

    ~ResonatorTypeComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    void handleAsyncUpdate() override;

    std::function<void(int value)> onChange;

private:
    juce::ComboBox resonatorTypeSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> resonatorTypeAttachment;
    juce::Label resonatorTypeLabel{"Resonator Type", "Resonator"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorTypeComponent)
};


#endif //PHYSICAL_EDUCATION_RESONATORTYPECOMPONENT_H

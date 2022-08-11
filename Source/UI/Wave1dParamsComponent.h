//
// Created by Tommy Rushton on 10/08/2022.
//

#ifndef PHYSICAL_EDUCATION_WAVE1DPARAMSCOMPONENT_H
#define PHYSICAL_EDUCATION_WAVE1DPARAMSCOMPONENT_H

#include <JuceHeader.h>

class Wave1dParamsComponent : public juce::Component {
public:
    Wave1dParamsComponent(
            juce::AudioProcessorValueTreeState &apvts,
            const juce::String &tensionID
    );

    ~Wave1dParamsComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

private:
    juce::Slider tensionSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tensionAttachment;
    juce::Label tensionLabel{"Tension", "Tension"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave1dParamsComponent)
};


#endif //PHYSICAL_EDUCATION_WAVE1DPARAMSCOMPONENT_H

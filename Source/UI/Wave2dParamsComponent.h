//
// Created by Tommy Rushton on 13/08/2022.
//

#ifndef PHYSICAL_EDUCATION_WAVE2DPARAMSCOMPONENT_H
#define PHYSICAL_EDUCATION_WAVE2DPARAMSCOMPONENT_H

#include <JuceHeader.h>

class Wave2dParamsComponent : public juce::Component {
public:
    Wave2dParamsComponent(
            juce::AudioProcessorValueTreeState &apvts,
            const juce::String &tensionID
    );

    ~Wave2dParamsComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

private:
    juce::Slider tensionSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tensionAttachment;
    juce::Label tensionLabel{"Tension", "Tension"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave2dParamsComponent)
};


#endif //PHYSICAL_EDUCATION_WAVE2DPARAMSCOMPONENT_H

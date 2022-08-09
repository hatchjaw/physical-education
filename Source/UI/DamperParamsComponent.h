//
// Created by Tommy Rushton on 24/04/2022.
//

#ifndef PHYSICAL_EDUCATION_DAMPERPARAMSCOMPONENT_H
#define PHYSICAL_EDUCATION_DAMPERPARAMSCOMPONENT_H

#include <JuceHeader.h>

class DamperParamsComponent : public juce::Component {
public:
    DamperParamsComponent(
            juce::AudioProcessorValueTreeState &apvts,
            const juce::String &damperPosID,
            const juce::String &damperStiffnessID,
            const juce::String &damperNonlinearityID,
            const juce::String &damperLossID
    );

    ~DamperParamsComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

private:
    juce::Label damperLabel{"Damper", "Damper"};

    juce::Slider damperPositionSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> damperPositionAttachment;
    juce::Label damperPositionLabel{"Damper Position", "Damper position"};

    juce::Slider damperStiffnessSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> damperStiffnessAttachment;
    juce::Label damperStiffnessLabel{"Damper Stiffness", "Stiffness"};

    juce::Slider damperNonlinearitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> damperNonlinearityAttachment;
    juce::Label damperNonlinearityLabel{"Damper Nonlinearity", "Nonlinearity"};

    juce::Slider damperDampingSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> damperDampingAttachment;
    juce::Label damperDampingLabel{"Damper Loss", "Damping"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DamperParamsComponent)
};


#endif //PHYSICAL_EDUCATION_DAMPERPARAMSCOMPONENT_H

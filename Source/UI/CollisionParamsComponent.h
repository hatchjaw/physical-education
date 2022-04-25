//
// Created by Tommy Rushton on 24/04/2022.
//

#ifndef PHYSICAL_EDUCATION_COLLISIONPARAMSCOMPONENT_H
#define PHYSICAL_EDUCATION_COLLISIONPARAMSCOMPONENT_H

#include <JuceHeader.h>

class CollisionParamsComponent : public juce::Component {
public:
    CollisionParamsComponent(
            juce::AudioProcessorValueTreeState &apvts,
            const juce::String &collisionPosID,
            const juce::String &collisionStiffnessID,
            const juce::String &collisionOmega1ID,
            const juce::String &collisionDampingID
    );

    ~CollisionParamsComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

private:
    juce::Label collisionLabel{"Collision", "Collision"};

    juce::Slider collisionPositionSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> collisionPositionAttachment;
    juce::Label collisionPositionLabel{"Collision Position", "Collision position"};

    juce::Slider collisionStiffnessSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> collisionStiffnessAttachment;
    juce::Label collisionStiffnessLabel{"Collision Stiffness", "Stiffness"};

    juce::Slider collisionOmega1Slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> collisionOmega1Attachment;
    juce::Label collisionOmega1Label{"Collision Omega1", "Omega"};

    juce::Slider collisionDampingSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> collisionDampingAttachment;
    juce::Label collisionDampingLabel{"Collision Damping", "Damping"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CollisionParamsComponent)
};


#endif //PHYSICAL_EDUCATION_COLLISIONPARAMSCOMPONENT_H

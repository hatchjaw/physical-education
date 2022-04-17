//
// Created by Tommy Rushton on 16/04/2022.
//

#ifndef PHYSICAL_EDUCATION_OUTPUTPOSITIONSCOMPONENT_H
#define PHYSICAL_EDUCATION_OUTPUTPOSITIONSCOMPONENT_H

#include <JuceHeader.h>
#include "../AudioProcessorValueTreeState/TwoValueSliderAttachment.h"

class OutputPositionsComponent : public juce::Component {
public:
    OutputPositionsComponent(juce::AudioProcessorValueTreeState &apvts,
                             const juce::String &outPos1ID,
                             const juce::String &outPos2ID);

    ~OutputPositionsComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

private:
    juce::Slider outputPositionsSlider;
    std::unique_ptr<TwoValueSliderAttachment> outputPositionsAttachment;
    juce::Label outputPositionsLabel{"Output Positions", "Output positions"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputPositionsComponent)
};


#endif //PHYSICAL_EDUCATION_OUTPUTPOSITIONSCOMPONENT_H

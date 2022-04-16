//
// Created by Tommy Rushton on 25/03/2022.
//

#ifndef PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H
#define PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H

#include <JuceHeader.h>
#include "../Utils.h"

class DisplacementVisualiserComponent : public juce::Component,
                                        public juce::Timer {
public:
    DisplacementVisualiserComponent(std::vector<FType> &);

    ~DisplacementVisualiserComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    // The timer callback function that updates the graphics.
    void timerCallback() override;

private:
    juce::Path generateStatePath();

    std::vector<FType> &displacement;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisplacementVisualiserComponent)
};


#endif //PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H

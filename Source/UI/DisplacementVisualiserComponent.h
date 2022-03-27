//
// Created by Tommy Rushton on 25/03/2022.
//

#ifndef PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H
#define PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H

#include <JuceHeader.h>

class DisplacementVisualiserComponent : public juce::Component,
                                        public Timer {
public:
    DisplacementVisualiserComponent(std::vector<double> &);

    ~DisplacementVisualiserComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    // The timer callback function that updates the graphics.
    void timerCallback() override;

    juce::Path visualiseState(juce::Graphics &);

private:
    std::vector<double> &displacement;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisplacementVisualiserComponent)
};


#endif //PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H

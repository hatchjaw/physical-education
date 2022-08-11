//
// Created by Tommy Rushton on 25/03/2022.
//

#ifndef PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H
#define PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H

#include <JuceHeader.h>
#include "../Utils.h"
#include "../Exciters/Exciter.h"
#include "../Resonators/Resonator.h"

class DisplacementVisualiserComponent : public juce::Component,
                                        public juce::Timer {
public:
    DisplacementVisualiserComponent(Resonator *&);

    ~DisplacementVisualiserComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    // The timer callback function that updates the graphics.
    void timerCallback() override;

private:
    const float PADDING_HORIZONTAL{10.f},
            DAMPER_INDICATOR_DIAMETER{15.f},
            BOW_INDICATOR_WIDTH{15.f},
            BOW_INDICATOR_HEIGHT{175.f};

    juce::Path generateStatePath();

    Resonator *&resonator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisplacementVisualiserComponent)

    juce::Path generateStatePathDynamic(std::vector<FType> &state);

    juce::Path generateStatePathStatic(std::vector<FType> &state);

    // TODO: parameterise the scaling factor.
    float gridSpacing{0.f}, scalingFactor{250.f};
};


#endif //PHYSICAL_EDUCATION_DISPLACEMENTVISUALISERCOMPONENT_H

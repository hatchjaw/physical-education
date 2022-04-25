//
// Created by Tommy Rushton on 25/03/2022.
//

#ifndef PHYSICAL_EDUCATION_PLUGINEDITOR_H
#define PHYSICAL_EDUCATION_PLUGINEDITOR_H

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/DisplacementVisualiserComponent.h"
#include "UI/OutputModeComponent.h"
#include "UI/OutputPositionsComponent.h"
#include "UI/ExcitationTypeComponent.h"
#include "UI/CollisionParamsComponent.h"

class PhysicalEducationAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit PhysicalEducationAudioProcessorEditor(PhysicalEducationAudioProcessor &);

    ~PhysicalEducationAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;

    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PhysicalEducationAudioProcessor &audioProcessor;

    // UI components.
//    juce::TextButton resetButton{"Reset"};
    DisplacementVisualiserComponent displacementVisualiserComponent;
    OutputModeComponent outputModeComponent;
    OutputPositionsComponent outputPositionsComponent;
    ExcitationTypeComponent excitationTypeComponent;
    CollisionParamsComponent collisionParamsComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhysicalEducationAudioProcessorEditor)
};


#endif //PHYSICAL_EDUCATION_PLUGINEDITOR_H

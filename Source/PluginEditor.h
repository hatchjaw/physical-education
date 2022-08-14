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
#include "UI/DamperParamsComponent.h"
#include "UI/ResonatorTypeComponent.h"
#include "UI/Wave1dParamsComponent.h"
#include "UI/Wave2dParamsComponent.h"

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
    ResonatorTypeComponent resonatorTypeComponent;
    DisplacementVisualiserComponent displacementVisualiserComponent;
    OutputModeComponent outputModeComponent;
    OutputPositionsComponent outputPositionsComponent;
    ExcitationTypeComponent excitationTypeComponent;
    DamperParamsComponent damperParamsComponent;
    Wave1dParamsComponent wave1dParamsComponent;
    Wave2dParamsComponent wave2dParamsComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhysicalEducationAudioProcessorEditor)
};


#endif //PHYSICAL_EDUCATION_PLUGINEDITOR_H

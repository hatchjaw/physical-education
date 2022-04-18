/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define NUM_VOICES 1

//==============================================================================
/**
*/
class PhysicalEducationAudioProcessor : public juce::AudioProcessor {
public:
    //==============================================================================
    PhysicalEducationAudioProcessor();

    ~PhysicalEducationAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

#endif

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const juce::String getProgramName(int index) override;

    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;

    void setStateInformation(const void *data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    /**
     *
     * @return A reference to the state of the underlying physical model,
     * e.g. for visualisation.
     */
    std::vector<double> &getModelState() noexcept;

private:
    juce::Synthesiser physEdSynth;
    /**
     * TODO: find a better way of checking the current exciter. Compare class
     *  names or something.
     */
    juce::String currentExciter;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhysicalEducationAudioProcessor)
};

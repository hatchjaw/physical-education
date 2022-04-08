/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PhysEdSound.h"
#include "PhysEdVoice.h"
#include "Resonators/StiffString.h"
#include "PluginEditor.h"
#include "Exciters/RaisedCosine.h"

//==============================================================================
PhysicalEducationAudioProcessor::PhysicalEducationAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
        : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                 .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
), apvts(*this, nullptr, "Parameters", PhysicalEducationAudioProcessor::createParams())
#endif
{
    physEdSynth.addSound(new PhysEdSound());

    for (int i = 0; i < NUM_VOICES; ++i) {
        // Create a voice.
        auto voice = new PhysEdVoice();
        voice->setResonator(new StiffString(new RaisedCosine()));
        // Add the voice to the synth.
        physEdSynth.addVoice(voice);
    }
}

PhysicalEducationAudioProcessor::~PhysicalEducationAudioProcessor() {
}

//==============================================================================
const juce::String PhysicalEducationAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool PhysicalEducationAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PhysicalEducationAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PhysicalEducationAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PhysicalEducationAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int PhysicalEducationAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int PhysicalEducationAudioProcessor::getCurrentProgram() {
    return 0;
}

void PhysicalEducationAudioProcessor::setCurrentProgram(int index) {
}

const juce::String PhysicalEducationAudioProcessor::getProgramName(int index) {
    return {};
}

void PhysicalEducationAudioProcessor::changeProgramName(int index, const juce::String &newName) {
}

//==============================================================================
void PhysicalEducationAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    physEdSynth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < physEdSynth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<PhysEdVoice *>(physEdSynth.getVoice(i))) {
            voice->prepareToPlay(sampleRate, samplesPerBlock, this->getTotalNumOutputChannels());
        }
    }
}

void PhysicalEducationAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations

bool PhysicalEducationAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

#endif

void PhysicalEducationAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                                   juce::MidiBuffer &midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Update parameters
//    for (int i = 0; i < physEdSynth.getNumVoices(); ++i) {
////        if (auto voice = dynamic_cast<peVoice *>(peSynth.getVoice(i))) {
////            voice->setExcitationMode(excitationMode);
////            voice->enableExcitationEnvelope(excitationEnvelopeOn);
////            voice->setExcitationEnvelope(excitationAdsrParams);
////            voice->updateSympatheticResonators(sympathetic1Freq,
////                                               sympathetic1Amount,
////                                               sympathetic2Freq,
////                                               sympathetic2Amount);
////            voice->updateStretchFactor(stretchFactor);
////            voice->updatePrimaryInharmonicity(inharmonicity1Gain, inharmonicity1Order);
////            voice->updateMutePrimary(sympatheticOnly);
////        }
//    }

    physEdSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool PhysicalEducationAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *PhysicalEducationAudioProcessor::createEditor() {
//    // Let JUCE create a generic UI
//    auto editor = new juce::GenericAudioProcessorEditor(*this);
//    editor->setSize(450, 585);
//    return editor;

    return new PhysicalEducationAudioProcessorEditor(*this);
}

//==============================================================================
void PhysicalEducationAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PhysicalEducationAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout
PhysicalEducationAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    return {params.begin(), params.end()};
}

//double *&PhysicalEducationAudioProcessor::getModelState() noexcept {
//    // Get first (only?) voice, then get the pointer to its state.
//    for (int i = 0; i < physEdSynth.getNumVoices(); ++i) {
//        if (auto voice = dynamic_cast<PhysEdVoice *>(physEdSynth.getVoice(i))) {
//            return voice->getResonatorState();
//        }
//    }
//}

std::vector<double> &PhysicalEducationAudioProcessor::getModelState() noexcept {
    // Get first (only?) voice, then get the pointer to its state.
    for (int i = 0; i < physEdSynth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<PhysEdVoice *>(physEdSynth.getVoice(i))) {
            return voice->getResonatorState();
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE

createPluginFilter() {
    return new PhysicalEducationAudioProcessor();
}

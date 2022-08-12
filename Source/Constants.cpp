//
// Created by Tommy Rushton on 18/04/2022.
//

#include "Constants.h"

const int Constants::Layout::OUTPUT_MODE_WIDTH = 225;
const int Constants::Layout::OUTPUT_POSITIONS_HEIGHT = 50;
const int Constants::Layout::EXCITATION_TYPE_WIDTH = 250;
const int Constants::Layout::EXCITATION_TYPE_HEIGHT = 30;
const int Constants::Layout::DAMPER_PARAMS_HEIGHT = 120;


const juce::StringRef Constants::ParameterIDs::RESONATOR_TYPE = "RESONATOR_TYPE";
const juce::StringRef Constants::ParameterIDs::EXCITATION_TYPE = "EXCITATION_TYPE";
const juce::StringRef Constants::ParameterIDs::OUTPUT_MODE = "OUTPUT_MODE";
const juce::StringRef Constants::ParameterIDs::OUT_POS_1 = "OUT_POS_1";
const juce::StringRef Constants::ParameterIDs::OUT_POS_2 = "OUT_POS_2";
const juce::StringRef Constants::ParameterIDs::FRICTION = "FRICTION";
const juce::StringRef Constants::ParameterIDs::DAMPER_POS = "DAMPER_POS";
const juce::StringRef Constants::ParameterIDs::DAMPER_STIFFNESS = "DAMPER_STIFFNESS";
const juce::StringRef Constants::ParameterIDs::DAMPER_NONLINEARITY = "DAMPER_OMEGA1";
const juce::StringRef Constants::ParameterIDs::DAMPER_LOSS = "DAMPER_LOSS";
const juce::StringRef Constants::ParameterIDs::TENSION = "TENSION";


const juce::StringArray Constants::RESONATOR_TYPES = {
        "Stiff String",
        "Dynamic 1D Wave",
        "Dynamic 2D Wave"
};

const juce::StringArray Constants::EXCITATION_TYPES = {
        "Raised Cosine",
        "Bow"
};

//
// Created by Tommy Rushton on 18/04/2022.
//

#ifndef PHYSICAL_EDUCATION_CONSTANTS_H
#define PHYSICAL_EDUCATION_CONSTANTS_H

#include <JuceHeader.h>

/**
 * Would like these to be compile-time constants (via static constexpr), but
 * that's not possible for juce::String, and std::string_view doesn't appear to
 * work either.
 */
struct Constants {
    struct Layout {
        static const int OUTPUT_MODE_WIDTH;
        static const int OUTPUT_POSITIONS_HEIGHT;
        static const int EXCITATION_TYPE_WIDTH;
        static const int EXCITATION_TYPE_HEIGHT;
        static const int DAMPER_PARAMS_HEIGHT;
    };

    struct ParameterIDs {
        static const juce::StringRef RESONATOR_TYPE;
        static const juce::StringRef EXCITATION_TYPE;
        static const juce::StringRef OUTPUT_MODE;
        static const juce::StringRef OUT_POS_1;
        static const juce::StringRef OUT_POS_2;
        static const juce::StringRef FRICTION;
        static const juce::StringRef DAMPER_POS;
        static const juce::StringRef DAMPER_STIFFNESS;
        static const juce::StringRef DAMPER_NONLINEARITY;
        static const juce::StringRef DAMPER_LOSS;
        static const juce::StringRef TENSION;
    };

    static const juce::StringArray RESONATOR_TYPES;
    static const juce::StringArray EXCITATION_TYPES;
};

#endif //PHYSICAL_EDUCATION_CONSTANTS_H

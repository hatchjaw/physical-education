//
// Created by Tommy Rushton on 18/04/2022.
//

#ifndef PHYSICAL_EDUCATION_CONSTANTS_H
#define PHYSICAL_EDUCATION_CONSTANTS_H

#include <JuceHeader.h>

struct Constants {
    struct Layout {
        static const int OUTPUT_MODE_WIDTH;
        static const int OUTPUT_POSITIONS_HEIGHT;
        static const int EXCITATION_TYPE_WIDTH;
    };

    struct ParameterIDs {
        static const juce::StringRef EXCITATION_TYPE;
        static const juce::StringRef OUTPUT_MODE;
        static const juce::StringRef OUT_POS_1;
        static const juce::StringRef OUT_POS_2;
        static const juce::StringRef FRICTION;
    };

    static const juce::StringArray EXCITATION_TYPES;
};

#endif //PHYSICAL_EDUCATION_CONSTANTS_H

//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef PHYSICAL_EDUCATION_PHYSEDSOUND_H
#define PHYSICAL_EDUCATION_PHYSEDSOUND_H

#include <JuceHeader.h>

class PhysEdSound : public juce::SynthesiserSound {
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }

    bool appliesToChannel(int midiChannel) override { return true; }
};


#endif //PHYSICAL_EDUCATION_PHYSEDSOUND_H

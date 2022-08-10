//
// Created by Tommy Rushton on 09/08/2022.
//

#include "DynamicResonator.h"

void DynamicResonator::initialiseState() {
    Utils::setupVectorPointers(u, uStates, stencilDimensions.second, Mu + 1);
    Utils::setupVectorPointers(w, wStates, stencilDimensions.second, parameters.derived.N - Mu + 1);
}

std::vector<FType> &DynamicResonator::getStaticState() {
    return wStates[0];
}

void DynamicResonator::advanceTimestep() {
    Resonator::advanceTimestep();
    Utils::pointerSwap(w);
}

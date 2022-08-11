//
// Created by Tommy Rushton on 25/04/2022.
//

#ifndef PHYSICAL_EDUCATION_SMOOTHEDPARAMETER_H
#define PHYSICAL_EDUCATION_SMOOTHEDPARAMETER_H

#include "../Utils.h"

template<typename T>
class SmoothedParameter {
public:
    explicit SmoothedParameter(T initialValue, T qVal = DEFAULT_Q);

    void set(T targetValue, bool skipSmoothing = false);

    T getNext();

    T &getCurrent();

private:
    static constexpr T DEFAULT_Q{.001f}, THRESHOLD{1e-6};
    T current, target, q;
};

#endif //PHYSICAL_EDUCATION_SMOOTHEDPARAMETER_H

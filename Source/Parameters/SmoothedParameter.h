//
// Created by Tommy Rushton on 25/04/2022.
//

#ifndef PHYSICAL_EDUCATION_SMOOTHEDPARAMETER_H
#define PHYSICAL_EDUCATION_SMOOTHEDPARAMETER_H

template<typename T>
class SmoothedParameter {
public:
    explicit SmoothedParameter(T initialValue);

    void set(T targetValue);

    T getNext();

    T &getCurrent();

private:
    static constexpr T MULTIPLIER{.1}, THRESHOLD{1e-6};
    T current, target;
};

template<typename T>
SmoothedParameter<T>::SmoothedParameter(T initialValue) {
    target = initialValue;
    current = target;
}

template<typename T>
void SmoothedParameter<T>::set(T targetValue) {
    target = targetValue;
}

template<typename T>
T SmoothedParameter<T>::getNext() {
    if (current != target) {
        auto delta = target - current;
        auto absDelta = abs(delta);
        if (absDelta < THRESHOLD) {
            current = target;
        } else {
            current += MULTIPLIER * delta;
        }
    }

    return current;
}

template<typename T>
T &SmoothedParameter<T>::getCurrent() {
    return current;
}

template
class SmoothedParameter<FType>;

template
class SmoothedParameter<float>;

#endif //PHYSICAL_EDUCATION_SMOOTHEDPARAMETER_H

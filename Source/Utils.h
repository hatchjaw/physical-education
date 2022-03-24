//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef NIME_PM_UTILS_H
#define NIME_PM_UTILS_H

#include <cmath>

class Utils {
public:
    /**
     * MATLAB-like modulo -- handles negative numbers.
     * @param a
     * @param b
     * @return
     */
    static int modulo(int a, int b) {
        return (b + (a % b)) % b;
    }

    static int wrapIndex(int index, int length) {
        if (index >= length) {
            index = 0;
        } else if (index < 0) {
            index = length - 1;
        }
        return index;
    }

    /**
     * Clamp a value with respect to an upper and lower bound.
     * Only makes sense if min \< max.
     * @param input
     * @param min
     * @param max
     * @return The clamped value
     */
    static float clamp(float input, float min, float max) {
        if (input < min) {
            input = min;
        } else if (input > max) {
            input = max;
        }

        return input;
    }
};

#endif //NIME_PM_UTILS_H

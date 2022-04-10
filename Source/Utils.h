//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef PHYSICAL_EDUCATION_UTILS_H
#define PHYSICAL_EDUCATION_UTILS_H

#include <cmath>

using FType = double;

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

    static float newtonRaphson() {
        return 0.f;
    }

    static double interpolate(const double *u, int readPos, double alpha) {
        return u[readPos - 1] * (-alpha * (alpha - 1) * (alpha - 2) / 6.0) +
               u[readPos] * ((alpha - 1) * (alpha + 1) * (alpha - 2) / 2.0) +
               u[readPos + 1] * (-alpha * (alpha + 1) * (alpha - 2) / 2.0) +
               u[readPos + 2] * (alpha * (alpha + 1) * (alpha - 1) / 6.0);
    }

    static void extrapolate(
            double *u,
            int writePos,
            double alpha,
            double gridSpacing,
            double excitation
    ) {
        auto hRecip = 1 / gridSpacing;
        u[writePos - 1] += hRecip * excitation * (-alpha * (alpha - 1) * (alpha - 2) / 6.0);
        u[writePos] += hRecip * excitation * ((alpha - 1) * (alpha + 1) * (alpha - 2) / 2.0);
        u[writePos + 1] += hRecip * excitation * (-alpha * (alpha + 1) * (alpha - 2) / 2.0);
        u[writePos + 2] += hRecip * excitation * (alpha * (alpha + 1) * (alpha - 1) / 6.0);
    }
};

#endif //PHYSICAL_EDUCATION_UTILS_H

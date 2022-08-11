//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef PHYSICAL_EDUCATION_UTILS_H
#define PHYSICAL_EDUCATION_UTILS_H

#include <cmath>
#include <vector>
#include <random>

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

    /**
     * NB, doesn't check whether indices are in-bounds.
     *
     * @param u Array to read from.
     * @param readPos Integral component of the read position.
     * @param alpha Fractional component of the read position.
     * @return
     */
    static FType interpolate(const FType *u, int readPos, FType alpha) {
        return u[readPos - 1] * (-alpha * (alpha - 1) * (alpha - 2) / 6.0) +
               u[readPos] * ((alpha - 1) * (alpha + 1) * (alpha - 2) / 2.0) +
               u[readPos + 1] * (-alpha * (alpha + 1) * (alpha - 2) / 2.0) +
               u[readPos + 2] * (alpha * (alpha + 1) * (alpha - 1) / 6.0);
    }

    static void extrapolate(
            FType *u,
            int writePos,
            FType alpha,
            FType gridSpacing,
            FType excitation
    ) {
        auto hRecip = 1 / gridSpacing;
        u[writePos - 1] += hRecip * excitation * (-alpha * (alpha - 1) * (alpha - 2) / 6.0);
        u[writePos] += hRecip * excitation * ((alpha - 1) * (alpha + 1) * (alpha - 2) / 2.0);
        u[writePos + 1] += hRecip * excitation * (-alpha * (alpha + 1) * (alpha - 2) / 2.0);
        u[writePos + 2] += hRecip * excitation * (alpha * (alpha + 1) * (alpha - 1) / 6.0);
    }

    /**
     * Set up a vector of vectors and a vector of pointers to the first element
     * in each vector in the vector of vectors. Try saying that quickly.
     * @param vectorOfPointers
     * @param vectorOfVectors
     * @param x First dimension, number of vectors of pointers.
     * @param y Second dimension, number of elements in the nested vectors.
     * @param init FType Initial value for elements in the nested vectors.
     */
    static void setupVectorPointers(
            std::vector<FType *> &vectorOfPointers,
            std::vector<std::vector<FType>> &vectorOfVectors,
            size_t x,
            size_t y,
            FType init = 0.0
    ) {
        vectorOfPointers.resize(x);
        vectorOfVectors.resize(x, std::vector<FType>(y, init));
        // Point each element in the vector of pointers to the address of the
        // start of the corresponding vector in the vector of vectors.
        for (unsigned long i = 0; i < x; ++i) {
            vectorOfPointers[i] = &vectorOfVectors[i][0];
        }
    }

    static void pointerSwap(std::vector<FType *> &vectorOfPointers) {
        auto end = vectorOfPointers.size() - 1;
        auto temp = vectorOfPointers[end];
        for (unsigned long i = end; i > 0; --i) {
            vectorOfPointers[i] = vectorOfPointers[i - 1];
        }
        vectorOfPointers[0] = temp;
    }

    static float randomFloat(float min, float max) {
        static std::default_random_engine rng;
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }
};

#endif //PHYSICAL_EDUCATION_UTILS_H

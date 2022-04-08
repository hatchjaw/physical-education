//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef NIME_PM_RESONATOR_H
#define NIME_PM_RESONATOR_H

#include <JuceHeader.h>
#include <vector>
#include "../Exciters/Exciter.h"

using FType = double;

class Resonator {
public:
    /**
     * Resonator constructor
     * @param stencil
     * @param exciterToUse
     */
    explicit Resonator(std::pair<unsigned int, unsigned int> stencil, Exciter *exciterToUse);

    void setDecayTimes(FType freqIndependent, FType freqDependent);

    /**
     * Set the output position for the model. Takes a float 0 ≤ p ≤ 1 and
     * calculates the corresponding l-value.
     * NB. N must be set before calling this method.
     *
     * @param outputPosition Normalised output position.
     */
    void setOutputPosition(float outputPosition);

    /**
     * Set the resonator's exciter.
     * @param exciterToUse
     */
    void setExciter(Exciter *exciterToUse);

    /**
     * Initialise the model -- compute coefficients and initialise state.
     *
     * @param sampleRate
     */
    virtual void initialiseModel(FType sampleRate);

    /**
     * Excite the system.
     *
     * @param position
     * @param force
     * @param velocity
     */
    void excite(float position = .5f, float force = 1.f, float velocity = 1.f);

    /**
     * Update the displacement of the resonator model by a single time-step.
     */
    virtual void updateState();

    /**
     * Get the current displacement of the output grid-point.
     *
     * @return The displacement of the output grid-point.
     */
    FType getOutput();

    /**
     *
     * @return A reference to a vector in vector of state vectors.
     */
    std::vector<FType> &getState();

protected:
    /**
     * Convert from a 60 dB decay time to a sigma value.
     * @param t60 decay time
     * @return sigma
     */
    static FType t60ToSigma(FType t60);

    /**
     * Compute the model coefficients.
     * NB. sample rate must be set before calling this method.
     */
    virtual void computeCoefficients() = 0;

    /**
     * Compute a single-timestep update of the scheme.
     */
    virtual void computeScheme() = 0;

    const FType L{1.0};
    /**
     * Dimensions of the model's grid stencil.
     */
    std::pair<unsigned int, unsigned int> stencilDimensions;
    /**
     * Model parameters. Time-step, 60 dB decay time, 60 dB frequency-dependent decay time.
     * NB. for stability, T60_1 should be greater than T60_0.
     */
    FType k, T60_0, T60_1;
    /**
     * Derived coefficients.
     */
    std::vector<FType> coeffs;
    /**
     * Number of position grid-points.
     */
    unsigned int N;
    /**
     * State pointers. To be treated as a circular buffer of states.
     */
    std::vector<FType *> u;
    std::vector<std::vector<FType>> uStates;

    bool isInitialised{false};
private:
    /**
     * Set the size of the state vectors and fill with zeros.
     */
    void initialiseState();

    /**
     * Advance the timestep by swapping pointers to state vectors.
     */
    void advanceTimestep();

    int outputIndex{0};

    Exciter *exciter;
};


#endif //NIME_PM_RESONATOR_H

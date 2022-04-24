//
// Created by Tommy Rushton on 23/03/2022.
//

#ifndef PHYSICAL_EDUCATION_RESONATOR_H
#define PHYSICAL_EDUCATION_RESONATOR_H

#include <JuceHeader.h>
#include <vector>
#include "../Utils.h"
#include "../Parameters/ModelParameters.h"
#include "../Exciters/Exciter.h"
#include "../Dampers/SpringDamper.h"

class Resonator {
public:
    enum OutputMode {
        DISPLACEMENT,
        VELOCITY,
    };

    /**
     * Resonator constructor
     * @param stencil
     */
    Resonator(std::pair<unsigned int, unsigned int> stencil, Exciter *exciterToUse);

    void setDecayTimes(FType freqIndependent, FType freqDependent);

    /**
     * Set the output positions for the model. Takes a vector of floats,
     * 0 ≤ p ≤ 1, and calculates the corresponding l-values.
     * NB. N must be set before calling this method.
     *
     * @param outputPositionToUse Normalised output positions. These will be clamped to 0 ≤ p ≤ 1.
     */
    void setOutputPositions(std::vector<float> outputPositionsToUse);

    /**
     * Set the output mode; use either DISPLACEMENT or VELOCITY.
     * @param mode
     */
    void setOutputMode(OutputMode mode);

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
     * Get the current displacement of the output positions.
     *
     * @return A vector containing the displacement of the output positions.
     */
    std::vector<FType> getOutput(unsigned long numOutputPositions);

    /**
     * Get the scaling factor for bringing the resonator's output into a useful
     * amplitude range.
     * @return
     */
    virtual FType getOutputScalar() = 0;

    /**
     * Get a reference to the resonator's 'current' state vector.
     * @return The current state vector.
     */
    std::vector<FType> &getState();

    /**
     * Get a reference to the resonator's parameter space.
     * @return
     */
    ModelParameters &getParameters();

    /**
     * Get a pointer to the exciter.
     * @return
     */
    Exciter *getExciter();

    void damp();

protected:
    /**
     * Convert from a 60 dB decay time to a frequency independent decay.
     * @param t60 decay time
     * @return sigma0
     */
    static FType t60ToSigma0(FType t60);

    FType t60ToSigma1(FType t60_0, FType t60_1, FType omega);

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
     * Model parameters.
     */
    ModelParameters parameters;
    /**
     * Derived coefficients.
     */
    std::vector<FType> coeffs;
    /**
     * State pointers. To be treated as a circular buffer of states.
     */
    std::vector<FType *> u;
    std::vector<std::vector<FType>> uStates;

    OutputMode outputMode{DISPLACEMENT};

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

    /**
     * Get the current displacement of a single output position.
     * @return The displacement of the output position.
     */
    FType getOutputAtPosition(unsigned long outputPositionIndex);

    /**
     * Positions from which to take displacement for output.
     */
    std::vector<float> normalisedOutputPositions;
    std::vector<float> outputPositions;
    /**
     * Buffers to store the most recent displacements for each output position.
     * To be used when using velocity for output.
     */
    std::vector<FType *> uOut;
    std::vector<std::vector<FType>> uOutStates;

    Exciter *exciter;

    SpringDamper damper;
};


#endif //PHYSICAL_EDUCATION_RESONATOR_H

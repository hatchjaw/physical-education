//
// Created by Tommy Rushton on 07/04/2022.
//

#ifndef PHYSICAL_EDUCATION_EXCITER_H
#define PHYSICAL_EDUCATION_EXCITER_H

#include <vector>

class Exciter {
public:
protected:
    virtual void initialiseExcitation(
            float excitationPosition,
            float excitationForce,
            float excitationVelocity
    );

    void setForce(float);

    void setVelocity(float);

    virtual void applyExcitation(std::vector<double *> &state) = 0;

    unsigned int N;
    float position{0.f}, force{0.f}, velocity{0.f};

private:
    friend class Resonator;

    void setNumGridPoints(unsigned int numGridPoints);
};


#endif //PHYSICAL_EDUCATION_EXCITER_H

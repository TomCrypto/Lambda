#ifndef BLACKBODY_H
#define BLACKBODY_H

#include <spectral/distribution.hpp>

/* This is a black-body emittance spectral power distribution. */
class BlackBody : public Distribution
{
    private:
        /* The black-body's temperature. */
        float temperature;
    public:
        /* This method creates a spectral distribution from scene file information. */
        BlackBody(std::fstream& file);

        /* This function returns the distribution value at a given wavelength. The exact nature of the result
         * depends on the type of spectral distribution (it could be power, refractive index, reflectance, ...) */
        virtual float Lookup(float wavelength);
};

#endif

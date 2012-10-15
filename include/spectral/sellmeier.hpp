#ifndef SELLMEIER_H
#define SELLMEIER_H

#include <spectral/distribution.hpp>

/* This is a Sellmeier-based spectral refractive index distribution. */
class Sellmeier : public Distribution
{
    private:
        /* The Sellmeier coefficients. */
        float B[3], C[3];
    public:
        /* This method creates a spectral distribution from scene file information. */
        Sellmeier(std::fstream& file);

        /* This function returns the distribution value at a given wavelength The exact nature of the result
         * depends on the type of spectral distribution (it could be power, refractive index, reflectance, ...) */
        virtual float Lookup(float wavelength);
};

#endif

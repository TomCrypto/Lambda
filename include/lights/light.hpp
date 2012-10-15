#ifndef LIGHT_H
#define LIGHT_H

/* Some scene file ID's for lights. */
#define ID_OMNI 0

/* We need vector math, and spectral distributions. */
#include <spectral/distribution.hpp>
#include <util/vec3.hpp>

/* This is the base class from which all light sources are derived. */
class Light
{
    public:
        /* This function returns the emittance of the light. */
        virtual double Emittance(Vector incident, Vector normal, double wavelength) = 0;
};

/* This creates the correct light type based on a scene file entity subtype. */
Light* GetLight(uint32_t subtype, std::fstream& file, std::vector<Distribution*>* distributions);

#endif // LIGHT_H


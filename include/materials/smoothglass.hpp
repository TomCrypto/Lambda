#ifndef SMOOTHGLASS_H
#define SMOOTHGLASS_H

#include <materials/material.hpp>

/* This is a smooth glass BRDF. It is assumed that the external refractive index is 1. */
class SmoothGlass : public Material
{
    private:
        /* Spectral refractive index distribution. */
        Distribution* refractiveIndex;
    public:
        /* Creates the material from a scene file. */
        SmoothGlass(std::fstream& file, std::vector<Distribution*>* distributions);

        /* This function returns an importance-sampled exitant vector. */
        virtual Vector Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng);

        /* This returns the reflectance for an incident and exitant vector. */
        virtual float Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled);
};

#endif

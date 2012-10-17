#ifndef COOKTORRANCE_H
#define COOKTORRANCE_H

#include <materials/material.hpp>

/* This is a Cook-Torrance BRDF. It is assumed that the external refractive index is 1. */
class CookTorrance : public Material
{
    private:
        /* Spectral reflectance and refractive index distribution. */
        Distribution* reflectance;
        Distribution* refractiveIndex;
        /* Surface roughness. */
        float roughness;
    public:
        /* Creates the material from a scene file. */
        CookTorrance(std::fstream& file, std::vector<Distribution*>* distributions);

        /* This function returns an importance-sampled exitant vector. */
        virtual Vector Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng);

        /* This returns the reflectance for an incident and exitant vector. */
        virtual float Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled);
};

#endif



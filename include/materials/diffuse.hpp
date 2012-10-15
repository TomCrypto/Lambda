#ifndef DIFFUSE_H
#define DIFFUSE_H

#include <materials/material.hpp>

/* This is a diffuse BRDF. */
class Diffuse : public Material
{
    private:
        /* Spectral reflectance distribution. */
        Distribution* reflectance;
    public:
        /* Creates the diffuse material from a scene file. */
        Diffuse(std::fstream& file, std::vector<Distribution*>* distributions);

        /* This function returns an importance-sampled exitant vector. */
        virtual Vector Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng);

        /* This returns the reflectance for an incident and exitant vector. */
        virtual float Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled);
};

#endif // DIFFUSE_H



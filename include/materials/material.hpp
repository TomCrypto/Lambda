/**
 * @file material.hpp
 *
 * \brief Material interface
 *
 * This is a common interface for materials, including BRDF's and BTDF's.
 */

#ifndef MATERIAL_H
#define MATERIAL_H

/* Some scene file ID's for materials. */
#define ID_DIFFUSE 0
#define ID_SPECULAR 1
#define ID_SMOOTHGLASS 2
#define ID_FROSTEDGLASS 3

/* We need vectors, pseudorandom number generation, and spectral distributions. */
#include <spectral/distribution.hpp>
#include <util/vec3.hpp>
#include <random>

/*! \class Material
 * This is the base class from which all materials are derived. */
class Material
{
    private:
    public:
        /*! This is the "outside" extinction coefficient (of the medium the primitive's normal is pointing towards). */
        float e1;
        /*! This is the "inside" extinction coefficient. */
        float e2;

        /*! Creates a material from a scene file. */
        Material(std::fstream& file);

        /*! This method returns an importance-sampled exitant light ray from an incident and normal vector. This
         * method is wavelength-dependent.
          \param origin A pointer to the intersection point.
          \param incident The incident vector.
          \param normal The surface normal.
          \param wavelength The ray's wavelength.
          \return Returns an importance-sampled exitant vector.
          \remark The origin will be slightly displaced by this method to prevent self-intersection due to
          floating-point inaccuracies. This is important to prevent geometry intersection artifacts. */
        virtual Vector Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng) = 0;

        /*! This method evaluates the material's reflectance function for an incident and exitant vector. This method
         * is wavelength-dependent.
          \param incident The incident vector.
          \param exitant The exitant vector.
          \param normal The surface normal.
          \param wavelength The ray's wavelength.
          \param sampled Whether the exitant vector was importance-sampled using the incident vector.
          \return Returns the incident-exitant reflectance.
          \remark This function must always return values in the interval [0, 1). Note 1 is exclusive, to ensure
          the light path always terminates eventually (and no surface reflects exactly 100% of incoming radiance). */
        virtual float Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled) = 0;
};

/* This creates the correct material type based on a scene file entity subtype. */
Material* GetMaterial(uint32_t subtype, std::fstream& file, std::vector<Distribution*>* distributions);

#endif

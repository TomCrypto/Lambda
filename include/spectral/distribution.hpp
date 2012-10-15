/**
 * @file distribution.hpp
 *
 * \brief Spectral distribution interface
 *
 * This is a common interface for spectral distributions, which are used to describe quantities that vary with
 * wavelength, such as reflectance, refractive index, absorption, etc... These basically behave as lookup tables
 * and can be implemented either analytically (such as the black-body emission spectrum) or derived from sampled
 * data.
 */

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

/* Some scene file ID's for spectral distributions. */
#define ID_BLACKBODY 0
#define ID_FLAT 1
#define ID_PEAK 2
#define ID_SELLMEIER 3

/* We need vector math and files. */
#include <util/vec3.hpp>
#include <iostream>
#include <fstream>
#include <vector>

/*! \class Distribution
 * This is the base class from which all spectral distributions are derived. */
class Distribution
{
    public:
        /*! This method evaluates the spectral distribution at any given wavelength.
        \param wavelength The wavelength to evaluate the distribution at.
        \return The value of the spectral distribution at the desired wavelength. The exact nature of this value
        depends on the type of the distribution.
        \remark The wavelength should be in nanometers and range between 380 and 780, i.e. the visible spectrum. */
        virtual float Lookup(float wavelength) = 0;
};

/* This creates the correct distribution type based on a scene file entity subtype. */
Distribution* GetDistribution(uint32_t subtype, std::fstream& file);

#endif

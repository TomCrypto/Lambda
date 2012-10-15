#include <spectral/sellmeier.hpp>

/* Sellmeier distribution scene file information. */
#pragma pack(1)
struct SellmeierDefinition
{
    /* The Sellmeier coefficients. */
    float B[3], C[3];
};

/* This method creates a spectral distribution from scene file information. */
Sellmeier::Sellmeier(std::fstream& file)
{
    /* Just read the definition. */
    SellmeierDefinition definition;
    file.read((char*)&definition, sizeof(SellmeierDefinition));

    /* And save the coefficients. */
    for (int t =0; t < 3; ++t)
    {
        this->B[t] = definition.B[t];
        this->C[t] = definition.C[t];
    }
}

float Sellmeier::Lookup(float wavelength)
{
    /* Convert the wavelength to micrometers. */
    wavelength *= 1e-3f;

    /* Compute the refractive index using the Sellmeier formula. */
    float index = 1.0f;
    for (int t = 0; t < 3; ++t) index += (this->B[t] * pow(wavelength, 2.0f)) / (pow(wavelength, 2.0f) - this->C[t]);
    return sqrt(index);
}


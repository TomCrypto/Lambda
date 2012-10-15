#include <spectral/blackbody.hpp>

/* This method creates a spectral distribution from scene file information. */
BlackBody::BlackBody(std::fstream& file)
{
    file.read((char*)&this->temperature, sizeof(float));
}

float BlackBody::Lookup(float wavelength)
{
    /* Convert the wavelength to meters. */
    wavelength *= 1e-9f;

    /* Apply Plank's Law. */
    float powerTerm  = 3.74183e-16f * pow(wavelength, -5.0f);
    return powerTerm / (exp(1.4388e-2f / (wavelength * this->temperature)) - 1.0f);
}

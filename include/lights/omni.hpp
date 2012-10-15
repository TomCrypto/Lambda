#ifndef OMNI_H
#define OMNI_H

#include <lights/light.hpp>

/* This is an omni light source with isotropic emittance. */
class Omni : public Light
{
    private:
        /* The spectral emittance spectrum. */
        Distribution* emittance;
    public:
        /* Creates the omni light from a scene file. */
        Omni(std::fstream& file, std::vector<Distribution*>* distributions);

        /* This function returns the emittance of the light. */
        virtual double Emittance(Vector incident, Vector normal, double wavelength) { return this->emittance->Lookup(wavelength); }
};

#endif // OMNI_H



#ifndef FLAT_H
#define FLAT_H

#include <spectral/distribution.hpp>

/* This is a flat (constant) spectral distribution. */
class Flat : public Distribution
{
    private:
        float constant;
    public:
        /* This method creates a spectral distribution from scene file information. */
        Flat(std::fstream& file){ file.read((char*)&this->constant, sizeof(float)); }
        virtual float Lookup(float wavelength){ return this->constant; }
};

#endif // FLAT_H



#ifndef PEAK_H
#define PEAK_H

#include <spectral/distribution.hpp>

/* This is a peak (almost monochromatic) spectral distribution. */
class Peak : public Distribution
{
    private:
        float peakWavelength;
    public:
        /* Specify the desired peak here. */
        Peak(std::fstream& file){ file.read((char*)&this->peakWavelength, sizeof(float)); }
        virtual float Lookup(float wavelength){ return exp(-pow(wavelength - this->peakWavelength, 2.0f) * 0.002f); }
};

#endif // PEAK_H




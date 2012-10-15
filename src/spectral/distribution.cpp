#include <spectral/distribution.hpp>

/* All spectral distribution types. */
#include <spectral/blackbody.hpp>
#include <spectral/flat.hpp>
#include <spectral/peak.hpp>
#include <spectral/sellmeier.hpp>

/* This creates the correct distribution type based on a scene file entity subtype. */
Distribution* GetDistribution(uint32_t subtype, std::fstream& file)
{
    switch (subtype)
    {
        case ID_BLACKBODY: return new BlackBody(file);
        case ID_FLAT: return new Flat(file);
        case ID_PEAK: return new Peak(file);
        case ID_SELLMEIER: return new Sellmeier(file);
    }

    /* Unknown subtype. */
    return nullptr;
}


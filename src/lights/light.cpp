#include <lights/light.hpp>

/* All light types. */
#include <lights/omni.hpp>

/* This creates the correct light type based on a scene file entity subtype. */
Light* GetLight(uint32_t subtype, std::fstream& file, std::vector<Distribution*>* distributions)
{
    switch(subtype)
    {
        case ID_OMNI: return new Omni(file, distributions);
    }

    /* Unknown subtype. */
    return nullptr;
}


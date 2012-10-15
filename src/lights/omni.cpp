#include <lights/omni.hpp>

/* Omni light scene file definition. */
#pragma pack(1)
struct OmniDefinition
{
    /* The index of the emittance distribution. */
    uint32_t emittance;
};

/* Creates the omni light from a scene file. */
Omni::Omni(std::fstream& file, std::vector<Distribution*>* distributions)
{
    /* Read the light definition from the scene file. */
    OmniDefinition definition;
    file.read((char*)&definition, sizeof(OmniDefinition));

    /* Get the appropriate distribution from the distribution vector. */
    this->emittance = distributions->at(definition.emittance);
}

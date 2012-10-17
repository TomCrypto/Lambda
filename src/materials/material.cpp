#include <materials/material.hpp>

/* All material types. */
#include <materials/diffuse.hpp>
#include <materials/specular.hpp>
#include <materials/smoothglass.hpp>
#include <materials/frostedglass.hpp>
#include <materials/cooktorrance.hpp>

/* This is a material scene file header. */
struct MaterialDefinition
{
    /* The outside and inside extinction coefficients. */
    float e1, e2;
};

/* Creates a material from a scene file. */
Material::Material(std::fstream& file)
{
    /* Read the material header from the scene file. */
    MaterialDefinition definition;
    file.read((char*)&definition, sizeof(MaterialDefinition));

    /* Set the appropriate class fields. */
    this->e1 = definition.e1;
    this->e2 = definition.e2;
}

/* This creates the correct material type based on a scene file entity subtype. */
Material* GetMaterial(uint32_t subtype, std::fstream& file, std::vector<Distribution*>* distributions)
{
    switch(subtype)
    {
        case ID_DIFFUSE: return new Diffuse(file, distributions);
        case ID_SPECULAR: return new Specular(file, distributions);
        case ID_SMOOTHGLASS: return new SmoothGlass(file, distributions);
        case ID_FROSTEDGLASS: return new FrostedGlass(file, distributions);
        case ID_COOKTORRANCE: return new CookTorrance(file, distributions);
    }

    /* Unknown subtype. */
    return nullptr;
}

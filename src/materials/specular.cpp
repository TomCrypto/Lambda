#include <materials/specular.hpp>

/* Specular material scene file definition. */
#pragma pack(1)
struct SpecularDefinition
{
    /* The index of the reflectance distribution. */
    uint32_t reflectance;
};

/* Creates the specular material from a scene file. */
Specular::Specular(std::fstream& file, std::vector<Distribution*>* distributions) : Material(file)
{
    /* Read the material definition from the scene file. */
    SpecularDefinition definition;
    file.read((char*)&definition, sizeof(SpecularDefinition));

    /* Get the appropriate distribution from the distribution vector. */
    this->reflectance = distributions->at(definition.reflectance);
}

Vector Specular::Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng)
{
    /* Align the normal with the incident vector. */
    if (incident * normal > 0) normal = ZERO - normal;

    /* Move the origin outside the surface slightly. */
    (*origin) = (*origin) + normal * EPSILON;

    /* Just return the reflected angle. */
    return reflect(incident, normal);
}

/* This returns the reflectance for an incident and exitant vector. */
float Specular::Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled)
{
    /* If the exitant ray was importance-sampled... */
    if (sampled)
    {
        /* Then we know it satisfies the law of reflection, so return the reflectance. */
        return std::max(this->reflectance->Lookup(wavelength), 0.0f);
    }
    else
    {
        /* Otherwise, find the expected reflected vector. */
        Vector expected = reflect(incident, normal);

        /* Return full reflectance if the exitant vector is the expected vector, otherwise zero. */
        if (!delta(1.0f - expected * exitant)) return 0.0f;
        else return std::max(this->reflectance->Lookup(wavelength), 0.0f);
    }
}


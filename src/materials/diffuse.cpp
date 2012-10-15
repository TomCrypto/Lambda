#include <materials/diffuse.hpp>

/* Diffuse material scene file definition. */
#pragma pack(1)
struct DiffuseDefinition
{
    /* The index of the reflectance distribution. */
    uint32_t reflectance;
};

/* Creates the diffuse material from a scene file. */
Diffuse::Diffuse(std::fstream& file, std::vector<Distribution*>* distributions) : Material(file)
{
    /* Read the material definition from the scene file. */
    DiffuseDefinition definition;
    file.read((char*)&definition, sizeof(DiffuseDefinition));

    /* Get the appropriate distribution from the distribution vector. */
    this->reflectance = distributions->at(definition.reflectance);
}

Vector Diffuse::Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng)
{
    /* Align the normal with the incident vector. */
    if (incident * normal > 0.0f) normal = ZERO - normal;

    /* Move the origin outside the surface slightly. */
    (*origin) = (*origin) + normal * EPSILON;

    /* Get two random numbers. */
    float u1 = RandomVariable(prng);
    float u2 = RandomVariable(prng);

    /* Compute a cosine-weighted vector. */
    float theta = 2.0f * PI * u2;
    float r = sqrtf(u1);

    /* Get the unit vector. */
    Vector direction = Vector(r * cosf(theta), sqrtf(1.0f - u1), r * sinf(theta));

    /* Rotate the vector with the normal. */
    return rotate(direction, normal);
}

/* This returns the reflectance for an incident and exitant vector. */
float Diffuse::Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled)
{
    /* If the exitant ray was importance-sampled... */
    if (sampled)
    {
        /* This is just a constant by definition, but we are using a cosine-weighted distribution so we divide
         * the reflectance by the cosine-weighted PDF (which happens to be cos(exitant, normal)). Note the
         * ubiquitous 1/pi factor happens to cancel out cleanly due to the sampling scheme. */
        return std::max(this->reflectance->Lookup(wavelength), 0.0f);
    }
    else
    {
        /* Otherwise, use the uniform sampling formulation. */
        return std::max(2.0f * this->reflectance->Lookup(wavelength) * std::abs(exitant * normal), 0.0f);
    }


}

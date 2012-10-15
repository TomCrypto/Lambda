#include <materials/frostedglass.hpp>

/* Frosted glass material scene file definition. */
#pragma pack(1)
struct FrostedGlassDefinition
{
    /* The index of the refractive index distribution. */
    uint32_t refractiveIndex;
    /* The roughness of the glass. */
    float roughness;
};

/* Creates the material from a scene file. */
FrostedGlass::FrostedGlass(std::fstream& file, std::vector<Distribution*>* distributions) : Material(file)
{
    /* Read the material definition from the scene file. */
    FrostedGlassDefinition definition;
    file.read((char*)&definition, sizeof(FrostedGlassDefinition));

    /* Get the appropriate distribution from the distribution vector. */
    this->refractiveIndex = distributions->at(definition.refractiveIndex);
    this->roughness = definition.roughness;
}

Vector FrostedGlass::Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng)
{
    /* Generate a random microfacet normal based on the Beckmann distribution with the given roughness. */
    float r1 = RandomVariable(prng);
    float r2 = RandomVariable(prng);
    float theta = atan(-pow(this->roughness, 2.0f) * log(1.0f - r1));
    float phi = 2.0f * PI * r2;
    Vector m = spherical(phi, theta);

    /* Rotate the microfacet normal according to the actual surface normal. */
    m = rotate(m, normal);

    /* Work out the correct n1 and n2 depending on the incident vector's direction relative to the normal. */
    float cosI = incident * normal;
    float n1, n2;
    if (cosI > 0)
    {
        /* Incident and normal have the same direction, ray is inside the material. */
        n1 = this->refractiveIndex->Lookup(wavelength);
        n2 = 1.0f;

        /* Flip the microfacet normal around. */
        m = ZERO - m;
    }
    else
    {
        /* Incident and normal have opposite directions, so the ray is outside the material. */
        n2 = this->refractiveIndex->Lookup(wavelength);
        n1 = 1.0f;

        /* Make the cosine positive. */
        cosI = -cosI;
    }

    /* Calculate the refracted angle's cosine. */
    float cosT = 1.0f - pow(n1 / n2, 2.0f) * (1.0f - pow(cosI, 2.0f));

    /* Check for total internal reflection. */
    if (cosT < 0.0f)
    {
        /* Total internal reflection occurred. */
        (*origin) = (*origin) + m * EPSILON;
        return reflect(incident, m);
    }

    /* Otherwise, finish computing the angle. */
    cosT = sqrt(cosT);

    /* Now, compute the Fresnel coefficients for reflection and refraction for a randomly polarized ray. */
    float R = (pow((n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT), 2.0f) + pow((n2 * cosI - n1 * cosT) / (n1 * cosT + n2 * cosI), 2.0f)) * 0.5f;

    /* Perform a random trial to decide whether to reflect or refract the ray. */
    if (RandomVariable(prng) < R)
    {
        /* Reflection. */
        (*origin) = (*origin) + m * EPSILON;
        return reflect(incident, m);
    }
    else
    {
        /* Refraction. */
        (*origin) = (*origin) - m * EPSILON;
        return incident * (n1 / n2) + m * ((n1 / n2) * cosI - cosT);
    }
}

/* This returns the reflectance for an incident and exitant vector. */
float FrostedGlass::Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled)
{
    /* If the exitant ray was importance sampled... */
    if (sampled)
    {
        /* Then the result is just going to be 1. */
        return 1.0f;
    }
    else
    {
        /* Otherwise, use delta functions to decide whether the incident and exitant rays are coherent. */
        // not done
        return 1.0f;
    }
}



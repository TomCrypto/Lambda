#include <materials/smoothglass.hpp>

/* Smooth glass material scene file definition. */
#pragma pack(1)
struct SmoothGlassDefinition
{
    /* The index of the refractive index distribution. */
    uint32_t refractiveIndex;
};

/* Creates the diffuse material from a scene file. */
SmoothGlass::SmoothGlass(std::fstream& file, std::vector<Distribution*>* distributions) : Material(file)
{
    /* Read the material definition from the scene file. */
    SmoothGlassDefinition definition;
    file.read((char*)&definition, sizeof(SmoothGlassDefinition));

    /* Get the appropriate distribution from the distribution vector. */
    this->refractiveIndex = distributions->at(definition.refractiveIndex);
}

Vector SmoothGlass::Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng)
{
    /* Work out the correct n1 and n2 depending on the incident vector's direction relative to the normal. */
    float cosI = incident * normal;
    float n1, n2;
    if (cosI > 0)
    {
        /* Incident and normal have the same direction, ray is inside the material. */
        n1 = this->refractiveIndex->Lookup(wavelength);
        n2 = 1.0f;

        /* Flip the normal around. */
        normal = ZERO - normal;
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
        (*origin) = (*origin) + normal * EPSILON;
        return reflect(incident, normal);
    }

    /* Otherwise, finish computing the angle. */
    cosT = sqrt(cosT);

    /* Now, compute the Fresnel coefficients for reflection and refraction for a randomly polarized ray. */
    float R = (pow((n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT), 2.0f) + pow((n2 * cosI - n1 * cosT) / (n1 * cosT + n2 * cosI), 2.0f)) * 0.5f;

    /* Perform a random trial to decide whether to reflect or refract the ray. */
    if (RandomVariable(prng) < R)
    {
        /* Reflection. */
        (*origin) = (*origin) + normal * EPSILON;
        return reflect(incident, normal);
    }
    else
    {
        /* Refraction. */
        (*origin) = (*origin) - normal * EPSILON;
        return incident * (n1 / n2) + normal * ((n1 / n2) * cosI - cosT);
    }
}

/* This returns the reflectance for an incident and exitant vector. */
float SmoothGlass::Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled)
{
    /* If the exitant ray was importance sampled... */
    if (sampled)
    {
        /* The reflectance will here be constant, because we've already weighted the
         * reflection/refraction probability according to the Fresnel equations. */
        return 1.0f;
    }
    else
    {
        /* Otherwise, use delta functions to decide whether the incident and exitant rays are coherent. */
        // not done
        return 0;
    }
}


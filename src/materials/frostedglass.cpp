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
    /* Work out the refractive indices. */
    float n1, n2;
    if (incident * normal)
    {
        /* Incident and normal have the same direction, ray is inside the material. */
        n1 = this->refractiveIndex->Lookup(wavelength);
        n2 = 1.0f;

        /* Flip the microfacet normal around. */
        normal = ZERO - normal;
    }
    else
    {
        /* Incident and normal have opposite directions, so the ray is outside the material. */
        n2 = this->refractiveIndex->Lookup(wavelength);
        n1 = 1.0f;
    }

    /* Check whether reflection or refraction occurred. */
    Vector H;
    float D = 1.0f;
    if (incident * exitant < 0.0f)
    {
        /* Reflection occurred, find the half-angle vector. */
        H = normalize(exitant - incident);

        /* If the ray was not importance-sampled, we need to take into account the distribution. */
        if (!sampled)
        {
            /* Get the half angle vector's angle with the normal. */
            float alpha = acos(H * normal);

            /* Compute the Beckmann distribution. */
            D = exp(-pow(tanf(alpha) / this->roughness, 2.0f));
        }
    }
    else
    {
        /* Refraction occurred, we have to find the microfacet normal. */
        float cI = std::abs(incident * normal);
        float cT = 1.0f - pow(n1 / n2, 2.0f) * (1.0f - pow(cI, 2.0f));
        H = (incident * (n1 / n2) - exitant) / ((n1 / n2) * cI - cT);

        /* If the ray was not importance-sampled, we need to take into account the distribution. */
        if (!sampled)
        {
            /* Get the half angle vector's angle with the normal. */
            float alpha = acos(H * normal);

            /* Compute the Beckmann distribution. */
            D = exp(-pow(tanf(alpha) / this->roughness, 2.0f));
        }
    }

    /* Compute the geometric attenuation term. */
    float NdV = std::abs(incident * normal);
    float NdL = std::abs(normal * exitant);
    float VdH = std::abs(incident * H);
    float NdH = std::abs(normal * H);
    float G = std::min(1.0f, std::min(2.0f * NdH * NdV / VdH, 2.0f * NdH * NdL / VdH));

    /* Compute the microfacet normalization term. */
    float norm = 1.0f / (PI * pow(this->roughness, 2.0f) * pow(NdH, 4.0f));

    /* Compute the reflectance (note the lambertian term cancels a dot product out).
     * Also note we do NOT use the fresnel term if the ray was importance-sampled,
     * since we were already weighting the probability of reflection and refraction
     * with it when sampling the BTDF. */
    return norm * (D * G) / (NdV);
}



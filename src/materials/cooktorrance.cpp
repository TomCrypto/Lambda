#include <materials/cooktorrance.hpp>

/* Cook-Torrance material scene file definition. */
#pragma pack(1)
struct CookTorranceDefinition
{
    /* The index of the refractive index distribution. */
    uint32_t reflectance, refractiveIndex;
    /* The roughness of the surface. */
    float roughness;
};

/* Creates the material from a scene file. */
CookTorrance::CookTorrance(std::fstream& file, std::vector<Distribution*>* distributions) : Material(file)
{
    /* Read the material definition from the scene file. */
    CookTorranceDefinition definition;
    file.read((char*)&definition, sizeof(CookTorranceDefinition));

    /* Get the appropriate distribution from the distribution vector. */
    this->reflectance = distributions->at(definition.reflectance);
    this->refractiveIndex = distributions->at(definition.refractiveIndex);
    this->roughness = definition.roughness;
}

Vector CookTorrance::Sample(Vector* origin, Vector incident, Vector normal, float wavelength, std::mt19937* prng)
{
    /* Align the normal with the incident vector. */
    if (incident * normal > 0.0f) normal = ZERO - normal;

    /* Move the origin outside the surface slightly. */
    (*origin) = (*origin) + normal * EPSILON;

    /* Generate a random microfacet normal based on the Beckmann distribution with the given roughness. */
    float r1 = RandomVariable(prng);
    float r2 = RandomVariable(prng);
    float theta = atan(-pow(this->roughness, 2.0f) * log(1.0f - r1));
    float phi = 2.0f * PI * r2;
    Vector m = spherical(phi, theta);
    m = rotate(m, normal);

    /* Reflect the incident vector accordingly. */
    return reflect(incident, m);
}

/* This returns the reflectance for an incident and exitant vector. */
float CookTorrance::Reflectance(Vector incident, Vector exitant, Vector normal, float wavelength, bool sampled)
{
    /* If the ray was not importance-sampled, we need to take into account the distribution. */
    float D = 1.0f;
    if (!sampled)
    {
        /* Get the exitant vector's angle with the normal. */
        float alpha = acos(exitant * normal);

        /* Compute the Beckmann distribution. */
        D = exp(-pow(tanf(alpha) / this->roughness, 2.0f));
    }

    /* Align the normal with the incident vector. */
    if (incident * normal > 0.0f) normal = ZERO - normal;

    /* Compute the half-angle vector. */
    Vector H = normalize(exitant - incident);

    /* Compute the refractive indices. */
    float n2 = this->refractiveIndex->Lookup(wavelength);
    float n1 = 1.0f;

    /* Compute the theoretical reflected and refracted angles. */
    float cosI = std::abs(incident * normal);
    float cosT = sqrtf(1.0f - pow(n1 / n2, 2.0f) * (1.0f - pow(cosI, 2.0f)));

    /* Compute the Fresnel term for the incident vector. */
    float F = (pow((n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT), 2.0f) + pow((n2 * cosI - n1 * cosT) / (n1 * cosT + n2 * cosI), 2.0f)) * 0.5f;

    /* Compute the geometric attenuation term. */
    float NdL = std::abs(normal * exitant);
    float VdH = std::abs(incident * H);
    float NdH = std::abs(normal * H);
    float NdV = cosI;
    float G = std::min(1.0f, std::min(2.0f * NdH * NdV / VdH, 2.0f * NdH * NdL / VdH));

    /* Compute the microfacet normalization term. */
    float norm = 1.0f / (PI * pow(this->roughness, 2.0f) * pow(NdH, 4.0f));

    /* Compute the reflectance (note the lambertian term cancels a dot product out). */
    return norm * this->reflectance->Lookup(wavelength) * (F * D * G) / (NdV);
}




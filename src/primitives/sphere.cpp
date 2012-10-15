#include <primitives/sphere.hpp>

/* This defines a sphere primitive. */
#pragma pack(1)
struct SphereDefinition
{
    float center[3];
    float radius;
};

/* Creates the sphere from a scene file. */
Sphere::Sphere(std::fstream& file, std::vector<Material*>* materials, std::vector<Light*>* lights) : Primitive(file, materials, lights)
{
    /* Read the sphere definition from the scene file. */
    SphereDefinition definition;
    file.read((char*)&definition, sizeof(SphereDefinition));

    /* Read the geometric center and radius of the sphere. */
    this->center = Vector(definition.center[0], definition.center[1], definition.center[2]);
    this->radius = definition.radius;

    /* Compute the sphere's radius squared. */
    this->radiusSquared = this->radius * this->radius;

    /* Compute the sphere's bounding box. */
    Vector radiusVector = Vector(this->radius, this->radius, this->radius);
    this->boundingBox = AABB(this->center - radiusVector, this->center + radiusVector);
}

/* Returns the closest intersection of a ray with the sphere, returns a negative value if no intersection. */
float Sphere::Intersect(const Ray& ray)
{
    Vector s = this->center - ray.o;
    float sd = s * ray.d;
    float ss = s * s;

    // Compute discriminant
    float disc = sd*sd - ss + this->radiusSquared;

    // Complex values: No intersection
    if( disc < 0.f ) return -1.0f;

    // Assume we are not in a sphere... The first hit is the lesser valued
    float p1 = sd - sqrt(disc);
    float p2 = sd + sqrt(disc);
    if (p1 < 0) return p2;
    if (p2 < 0) return p1;
    return std::min(p1, p2);

    //return sd - sqrt(disc);

    #if 0

    /* Transform the ray's origin into the sphere's object space. */
    Vector origin = ray.o - this->definition.center;

    /* Compute the intersection quadratic equation's coefficients. */
    float a = ray.d * ray.d;
    float b = 2 * (ray.d * origin);
    float c = (origin * origin) - this->radiusSquared;

    /* Compute the squared discriminant to discard the no-intersection case. */
    float delta = b * b - 4 * a * c;
    if (delta < 0) return false;

    /* Otherwise, take the square root of the discriminant and solve the quadratic. */
    delta = sqrtf(delta);
    float p1 = (-b - delta) / (2 * a);
    float p2 = (-b + delta) / (2 * a);

    /* Use short-circuit rejection to eliminate cases one by one. */
    if ((p1 <= EPSILON) && (p2 <= EPSILON)) return false;
    intersection->object = this;

    if (p1 <= EPSILON) intersection->t = p2; else
    if (p2 <= EPSILON) intersection->t = p1; else
    intersection->t = std::min(p1, p2);
    return (intersection->t >= EPSILON);
    #endif
}

/* Returns the surface normal at any point on the sphere. */
Vector Sphere::Normal(Vector point)
{
    return (point - this->center) / this->radius;
}

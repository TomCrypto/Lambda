#include <primitives/triangle.hpp>

/* This defines a triangle. */
#pragma pack(1)
struct TriangleDefinition
{
    /* The three vertices. */
    float p1[3], p2[3], p3[3];
};

/* Creates the triangle from a scene file. */
Triangle::Triangle(std::fstream& file, std::vector<Material*>* materials, std::vector<Light*>* lights) : Primitive(file, materials, lights)
{
    /* Read the triangle definition from the scene file. */
    TriangleDefinition definition;
    file.read((char*)&definition, sizeof(TriangleDefinition));

    /* Read the vertices from the definition. */
    this->p1 = Vector(definition.p1[0], definition.p1[1], definition.p1[2]);
    this->p2 = Vector(definition.p2[0], definition.p2[1], definition.p2[2]);
    this->p3 = Vector(definition.p3[0], definition.p3[1], definition.p3[2]);

    /* Precompute the normal and edges. */
    edge1 = this->p2 - this->p1;
    edge2 = this->p3 - this->p1;
    normal = normalize(edge1 ^ edge2);

    /* Compute the triangle's bounding box. */
    Vector lo = Vector(std::min(this->p1.x, std::min(this->p2.x, this->p3.x)),
                       std::min(this->p1.y, std::min(this->p2.y, this->p3.y)),
                       std::min(this->p1.z, std::min(this->p2.z, this->p3.z)));
    Vector hi = Vector(std::max(this->p1.x, std::max(this->p2.x, this->p3.x)),
                       std::max(this->p1.y, std::max(this->p2.y, this->p3.y)),
                       std::max(this->p1.z, std::max(this->p2.z, this->p3.z)));
    this->boundingBox = AABB(lo, hi);

    /* Compute the triangle's centroid. */
    this->centroid = (this->p1 + this->p2 + this->p3) / 3.0f;
}
/* Returns the closest intersection of a ray with the triangle, returns a negative value if no intersection. */
float Triangle::Intersect(const Ray& ray)
{
    /* Compute some initial values. */
    Vector distance = ray.o - this->p1;
    Vector s = ray.d ^ edge2;
    float d = 1.0 / (s * edge1);

    /* Calculate the first barycentric coordinate. */
    float u = (distance * s) * d;

    /* Reject the intersection if the barycentric coordinate is out of range. */
    if ((u <= -EPSILON) || (u >= 1 + EPSILON)) return -1.0f;

    /* Calculate the second barycentric coordinate. */
    s = distance ^ edge1;
    float v = (ray.d * s) * d;

    /* Reject the intersection if the barycentric coordinate is out of range. */
    if ((v <= -EPSILON) || (u + v >= 1 + EPSILON)) return -1.0f;

    /* Compute the final intersection point. */
    return (edge2 * s) * d;
}

/* Returns the surface normal at any point on the triangle. */
Vector Triangle::Normal(Vector point)
{
    return normal;
}

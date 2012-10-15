#ifndef SPHERE_H
#define SPHERE_H

#include <primitives/primitive.hpp>

class Sphere : public Primitive
{
    private:
        /* The sphere's center. */
        Vector center;

        /* The sphere's radius. */
        float radius;

        /* The sphere's square radius. */
        float radiusSquared;

        /* The sphere's bounding box. */
        AABB boundingBox;
    public:
        /* Creates the sphere from a scene file. */
        Sphere(std::fstream& file, std::vector<Material*>* materials, std::vector<Light*>* lights);

        /* This function returns the closest intersection of a ray with the sphere. */
        virtual float Intersect(const Ray& ray);

        /* This function returns the surface normal of the sphere at a given point. */
        virtual Vector Normal(Vector point);

        /* This function returns the bounding box of the sphere. */
        virtual AABB BoundingBox(){ return this->boundingBox; }

        /* This function returns the centroid of the sphere. */
        virtual Vector Centroid(){ return this->center; }
};

#endif // SPHERE_H

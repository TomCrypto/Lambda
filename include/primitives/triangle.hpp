#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <primitives/primitive.hpp>

class Triangle : public Primitive
{
    private:
        /* The triangle's three vertices. */
        Vector p1, p2, p3;

        /* The triangle's precomputed edges, normal and centroid. */
        Vector edge1, edge2, normal, centroid;

        /* The triangle's bounding box. */
        AABB boundingBox;
    public:
        /* Creates the triangle from a scene file. */
        Triangle(std::fstream& file, std::vector<Material*>* materials, std::vector<Light*>* lights);

        /* This function returns the closest intersection of a ray with the triangle. */
        virtual float Intersect(const Ray& ray);

        /* This function returns the surface normal of the triangle at a given point. */
        virtual Vector Normal(Vector point);

        /* This function returns the bounding box of the triangle. */
        virtual AABB BoundingBox(){ return this->boundingBox; }

        /* This function returns the centroid of the triangle. */
        virtual Vector Centroid(){ return this->centroid; }
};

#endif // TRIANGLE_H


/**
 * @file primitive.hpp
 *
 * \brief Geometric primitive interface
 *
 * This is a common interface for geometric primitives, such as triangles, spheres, etc...
 * To be implementable, all primitives need to fulfill the following conditions: \n
 * - the primitive must be bounded (infinitely large primitives such as planes are not supported as they cannot be
 * queried by the bounding volume hierarchy acceleration structure). \n
 * - there must exist a computable ray-primitive intersection algorithm for this primitive. It need not be analytical,
 * but it probably should be for performance. \n
 * - if the primitive is to be used as a light source, it must be possible to select a random point on the primitive's
 * surface uniformily.
 * - the surface normal at any point on the primitive's surface must be computable. \n
 * - the primitive must have a well-defined centroid.
 */

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

/* Some scene file ID's for primitives. */
#define ID_SPHERE 0
#define ID_TRIANGLE 1

/* We need vector and AABB math, as well as material and light references. */
#include <materials/material.hpp>
#include <lights/light.hpp>
#include <util/vec3.hpp>
#include <util/aabb.hpp>

/* Forward declaration for the intersection information record. */
class Primitive;

/*! \brief Ray-geometry intersection record.
 *
 * This is a structure containing information about a ray-geometry intersection. It contains a reference to the
 * primitive intersected, and the intersection's distance along the ray, starting from the ray's origin. */
struct Intersection {
 /*! The primitive which was intersected. */
 Primitive* primitive;
 /*! The intersection's distance. */
 float t;
};

/*! \class Primitive
 * This is the base class from which all geometric primitives are derived. */
class Primitive
{
    public:
        /*! Points to the primitive's material, if it has one. */
        Material* material;
        /*! Points to the primitive's light, if it has one. */
        Light* light;

        /*! Creates a primitive, from a scene file and a list of materials and lights. */
        Primitive(std::fstream& file, std::vector<Material*>* materials, std::vector<Light*>* lights);

        /*! This method returns the closest intersection of a ray with the primitive.
         \param ray The ray to test intersection with.
         \returns The closest distance along the ray where an intersection occurs. If this is a negative value, the
         ray does not intersect the primitive. */
        virtual float Intersect(const Ray& ray) = 0;

        /*! This method returns the surface normal of the primitive at any given point on its surface.
         \param point The point, on the primitive's surface, to obtain the normal of.
         \return The surface normal at the desired point. */
        virtual Vector Normal(Vector point) = 0;

        /*! This method returns the axis-aligned bounding box of the primitive.
         \return The primitive's bounding box.
         \remark The bounding box need not be ideal, but the bounding volume hierarchy is more efficient if the
         returned bounding box tightly fits the primitive. The same bounding box must always be returned. */
        virtual AABB BoundingBox() = 0;

        /*! This method returns the centroid of the primitive.
         \return The primitive's centroid.
         \remark If the centroid is not well-defined, pass the best one and the bounding volume hierarchy will do its
         best to handle it. The same centroid must always be returned. */
        virtual Vector Centroid() = 0;
};

/* This creates the correct primitive type based on a scene file entity subtype. */
Primitive* GetPrimitive(uint32_t subtype, std::fstream& file, std::vector<Material*>* materials, std::vector<Light*>* lights);

#endif

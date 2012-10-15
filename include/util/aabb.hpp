// from https://github.com/brandonpelfrey/Fast-BVH

#ifndef BBox_h
#define BBox_h

#include <util/vec3.hpp>
#include <stdint.h>

struct AABB {
	Vector min, max, extent;
	AABB() { }
	AABB(const Vector& min, const Vector& max);
	AABB(const Vector& p);

	bool intersect(const Ray& ray, float *tnear, float *tfar) const;
 void expandToInclude(const Vector& p);
 void expandToInclude(const AABB& b);
 uint32_t maxDimension() const;
 float surfaceArea() const;
};

#endif

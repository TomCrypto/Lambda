#ifndef BVH_h
#define BVH_h

#include <util/aabb.hpp>
#include <vector>
#include <stdint.h>
#include <primitives/primitive.hpp>

//! Node descriptor for the flattened tree
struct BVHFlatNode {
 AABB bbox;
 uint32_t start, nPrims, rightOffset;
};

//! \author Brandon Pelfrey
//! A Bounding Volume Hierarchy system for fast Ray-Object intersection tests
class BVH {
 uint32_t leafSize;
 std::vector<Primitive*>* build_prims;

 //! Build the BVH tree out of build_prims
 void build();

 // Fast Traversal System
 BVHFlatNode *flatTree;

public:
 uint32_t nNodes, nLeafs;
 BVH(std::vector<Primitive*>* objects, uint32_t leafSize=4);
 bool getIntersection(const Ray& ray, Intersection *intersection, bool occlusion) const ;

 ~BVH();
};

#endif

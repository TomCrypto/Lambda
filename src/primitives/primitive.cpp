#include <primitives/primitive.hpp>

/* All primitive types. */
#include <primitives/sphere.hpp>
#include <primitives/triangle.hpp>

/* This is a primitive scene file header. */
struct PrimitiveDefinition
{
    /* The primitive's material (negative if it has none). */
    int32_t material;
    /* The primitive's light (negative if it has none). */
    int32_t light;
};

/* Creates a primitive, from a scene file and a list of materials and lights. */
Primitive::Primitive(std::fstream& file, std::vector<Material*>* materials, std::vector<Light*>* lights)
{
    /* Read the primitive header from the scene file. */
    PrimitiveDefinition definition;
    file.read((char*)&definition, sizeof(PrimitiveDefinition));

    /* Set the appropriate materials and lights from the passed vectors. */
    this->material = (definition.material >= 0) ? materials->at(definition.material) : nullptr;
    this->light = (definition.light >= 0) ? lights->at(definition.light) : nullptr;
}

/* This creates the correct primitive type based on a scene file entity subtype. */
Primitive* GetPrimitive(uint32_t subtype, std::fstream& file, std::vector<Material*>* materials, std::vector<Light*>* lights)
{
    switch(subtype)
    {
        case ID_SPHERE: return new Sphere(file, materials, lights);
        case ID_TRIANGLE: return new Triangle(file, materials, lights);
    }

    /* Unknown subtype. */
    return nullptr;
}

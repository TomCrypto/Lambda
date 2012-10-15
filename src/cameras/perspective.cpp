#include <cameras/perspective.hpp>

/* Perspective CG camera definition. */
#pragma pack(1)
struct PerspectiveCGDefinition
{
    /* Camera position and target, and field of view (in radians). */
    float pos[3], tar[3];
    float fieldOfView;
};

/* Creates the perspective camera from a scene file. */
Perspective::Perspective(std::fstream& file)
{
    /* Read the camera definition from the scene file. */
    PerspectiveCGDefinition definition;
    file.read((char*)&definition, sizeof(PerspectiveCGDefinition));

    /* Save the camera's position. */
    this->position = Vector(definition.pos[0], definition.pos[1], definition.pos[2]);
    Vector target = Vector(definition.tar[0], definition.tar[1], definition.tar[2]);

    /* Build the focal plane. */
    buildFocalPlane(target, definition.fieldOfView);
}

void Perspective::buildFocalPlane(Vector target, float fieldOfView)
{
    /* Define the orthonormal basis for this lookat vector. */
    Vector zAxis = normalize(target - this->position);
    Vector xAxis = normalize(UPWARDS ^ zAxis);
    Vector yAxis = normalize(zAxis ^ xAxis);

    /* Compute the inverse view matrix from the orthonormal basis. */
    Vector inverseViewMatrix[3] = {
        Vector{xAxis.x, yAxis.x, zAxis.x},
        Vector{xAxis.y, yAxis.y, zAxis.y},
        Vector{xAxis.z, yAxis.z, zAxis.z}
    };

    /* Scale the field of view accordingly. */
    float FOV = tan(fieldOfView * 0.5f);

    /* Compute the focal plane corners. */
    Vector multiplicand = Vector(-FOV, -FOV, 1);
    this->focalPlane[0] = Vector(multiplicand * inverseViewMatrix[0],
                                  multiplicand * inverseViewMatrix[1],
                                  multiplicand * inverseViewMatrix[2]);

    multiplicand = Vector(+FOV, -FOV, 1);
    this->focalPlane[1] = Vector(multiplicand * inverseViewMatrix[0],
                                  multiplicand * inverseViewMatrix[1],
                                  multiplicand * inverseViewMatrix[2]);

    multiplicand = Vector(+FOV, +FOV, 1);
    this->focalPlane[2] = Vector(multiplicand * inverseViewMatrix[0],
                                  multiplicand * inverseViewMatrix[1],
                                  multiplicand * inverseViewMatrix[2]);

    multiplicand = Vector(-FOV, +FOV, 1);
    this->focalPlane[3] = Vector(multiplicand * inverseViewMatrix[0],
                                  multiplicand * inverseViewMatrix[1],
                                  multiplicand * inverseViewMatrix[2]);
}

/* Traces the camera ray. */
Ray Perspective::Trace(float u, float v)
{
    /* Simply interpolate the ray's direction. */
    return Ray(this->position,
               normalize(lerp(lerp(this->focalPlane[0], this->focalPlane[1], (u + 1.0f) * 0.5f),
                              lerp(this->focalPlane[3], this->focalPlane[2], (u + 1.0f) * 0.5f), (1.0f - v) * 0.5f)));
}

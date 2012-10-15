#ifndef CAMERA_H
#define CAMERA_H

/* Some scene file ID's for cameras. */
#define ID_PERSPECTIVE 0

/* We need vector math, and files. */
#include <util/vec3.hpp>
#include <iostream>
#include <fstream>

/* This is the base class for a camera. */
class Camera
{
    public:
        /* This function returns the camera ray corresponding to the normalized screen coordinates (u, v). */
        virtual Ray Trace(float u, float v) = 0;
};

/* This creates the correct camera type based on a scene file entity subtype. */
Camera* GetCamera(uint32_t subtype, std::fstream& file);

#endif

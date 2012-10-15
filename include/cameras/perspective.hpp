#ifndef PERSPECTIVECG_H
#define PERSPECTIVECG_H

#include <cameras/camera.hpp>

/* This represents a standard perspective camera, with no advanced features such as depth of field. */
class Perspective : public Camera
{
    private:
        /* The camera position. */
        Vector position;

        /* The focal plane. */
        Vector focalPlane[4];

        /* Build the focal plane. */
        void buildFocalPlane(Vector target, float fieldOfView);
    public:
        /* Creates the perspective camera from a scene file. */
        Perspective(std::fstream& file);

        /* This will trace the camera ray. */
        virtual Ray Trace(float u, float v);
};

#endif // PERSPECTIVECG_H

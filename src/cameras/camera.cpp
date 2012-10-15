#include <cameras/camera.hpp>

/* All camera types. */
#include <cameras/perspective.hpp>

/* This creates the correct camera type based on a scene file entity subtype. */
Camera* GetCamera(uint32_t subtype, std::fstream& file)
{
    switch (subtype)
    {
        case ID_PERSPECTIVE: return new Perspective(file);
    }

    /* Unknown subtype. */
    return nullptr;
}

/**
 * @file renderer.hpp
 *
 * \brief Renderer interface
 *
 * This is the renderer's interface.
 */

#ifndef RENDERER_H
#define RENDERER_H

/* We'll need every single header file. */
#include <primitives/primitive.hpp>
#include <primitives/triangle.hpp>
#include <primitives/sphere.hpp>
#include <materials/material.hpp>
#include <materials/specular.hpp>
#include <materials/diffuse.hpp>
#include <materials/smoothglass.hpp>
#include <materials/frostedglass.hpp>
#include <lights/light.hpp>
#include <lights/omni.hpp>
#include <cameras/camera.hpp>
#include <cameras/perspective.hpp>
#include <scenegraph/bvh.hpp>
#include <spectral/distribution.hpp>
#include <spectral/blackbody.hpp>
#include <spectral/flat.hpp>
#include <spectral/peak.hpp>
#include <spectral/sellmeier.hpp>
#include <util/aabb.hpp>
#include <util/vec3.hpp>
#include <util/cie.hpp>

/* And a few standard includes, too. */
#include <vector>
#include <random>

/*! This contains global rendering information such as the width and height of the render. */
#pragma pack(1)
struct RenderParams
{
    /*! The width of the render. */
    int32_t width;
    /*! The height of the render. */
    int32_t height;
    /*! THe number of samples in the render. */
    int32_t samples;
};

/*! \class Renderer
 * This is the main renderer class which drives the rendering algorithm. */
class Renderer
{
    private:
        /*! This is a list of all spectral distributions in use. */
        std::vector<Distribution*>* distributions;
        /*! This is a list of all the primitives in the scene. */
        std::vector<Primitive*>* primitives;
        /*! These are all the materials used in the scene. */
        std::vector<Material*>* materials;
        /*! These are all the lights used in the scene. */
        std::vector<Light*>* lights;
        /*! The render parameters to use to render. */
        RenderParams renderParams;
        /*! The color system to use for rendering. */
        ColorSystem colorSystem;
        /*! This is the camera to render from. */
        Camera* camera;
        /*! This is the bounding volume hierarchy. */
        BVH* bvh;
        /*! This applies the Reinhard tonemapping operator to a pixel array. */
        void TonemapRender(Vector* pixels);
        /*! This gamma-corrects a pixel array. */
        void GammaCorrectRender(Vector* pixels);
        /*! Saves a pixel array to a PPM file. */
        void SaveToPPM(Vector* pixels, std::string render, time_t elapsedTime);
        /*! Returns a radiance sample along a light ray. */
        float Radiance(Ray ray, float wavelength, std::mt19937* prng);
        /*! Number of pixels in the render. */
        size_t pixelCount;
    public:
        /*! This constructor initializes the renderer from a scene file.
         \param scene The scene file to open. */
        Renderer(std::string scene);

        /*! This method renders the scene into a PPM file.
          \param threads The number of threads to use. */
        void Render(std::string render, size_t threads);

        /*! This destructor will free all resources used by the renderer. */
        ~Renderer();
};

#endif

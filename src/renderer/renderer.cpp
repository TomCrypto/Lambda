/* Bunch of required includes (most of them are in the header). */
#include <renderer/renderer.hpp>
#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>
#include <omp.h>

/* Some OpenMP convenience macros. */
#define threadID omp_get_thread_num()

/* This is the BVH's leaf size. It should be set for best performance,
 * but this heavily depends on hardware factors. 2 is usually best. */
#define LEAFSIZE 2

/* These are scene entity types, which indicate the nature of the next object in the scene file. */
enum EntityType { COLORSYSTEM = 0, CAMERA = 1, DISTRIBUTION = 2, MATERIAL = 3, LIGHT = 4, PRIMITIVE = 5 };

/* This is an entity header. */
#pragma pack(1)
struct EntityHeader
{
    /* The entity type. */
    EntityType type;
    /* The subtype. */
    uint32_t subtype;
};

/* Just for readability. */
using namespace std;

/* Utility function to read a scene file entity header from a file. */
bool ReadHeader(fstream& file, EntityHeader* header)
{
    file.read((char*)header, sizeof(EntityHeader));
    return (!file.eof());
}

Renderer::Renderer(string scene)
{
    /* Open the scene file. */
    fstream file;
    file.open(scene, ios::in | ios::binary);
    cout << "[+] Opening <" << scene << ">..." << endl << endl;

    /* Read the render information. */
    file.read((char*)&renderParams, sizeof(RenderParams));
    cout << "[+] Scene file header:" << endl;
    cout << "    | " << renderParams.width << "×" << renderParams.height << "." << endl;
    cout << "    | " << renderParams.samples << " spp." << endl;
    cout << endl << "[+] Parsing scene entities..." << flush;
    pixelCount = renderParams.width * renderParams.height;

    /* Create all the storage vectors. */
    distributions = new vector<Distribution*>();
    primitives = new vector<Primitive*>();
    materials = new vector<Material*>();
    lights = new vector<Light*>();

    /* Read every scene entity in the file. */
    EntityHeader header;
    while (ReadHeader(file, &header))
    {
        /* Check the entity type to know what to do. */
        switch(header.type)
        {
            case DISTRIBUTION: distributions->push_back(GetDistribution(header.subtype, file)); break;
            case     MATERIAL: materials->push_back(GetMaterial(header.subtype, file, distributions)); break;
            case        LIGHT: lights->push_back(GetLight(header.subtype, file, distributions)); break;
            case    PRIMITIVE: primitives->push_back(GetPrimitive(header.subtype, file, materials, lights)); break;
            case  COLORSYSTEM: colorSystem = ColorSystems[header.subtype]; break;
            case       CAMERA: camera = GetCamera(header.subtype, file); break;
        }
    }

    /* Print out statistics. */
    cout << " complete!" << endl << endl << "[+] Scene statistics:" << endl;
    cout << "    | " << primitives->size() << " geometric primitive(s)." << endl;
    cout << "    | " << distributions->size() << " spectral distribution(s)." << endl;
    cout << "    | " << materials->size() << " material(s)." << endl;
    cout << "    | " << lights->size() << " light(s)." << endl;

    /* Build the bounding volume hierarchy. */
    cout << endl << "[+] Building acceleration structure..." << flush;
    bvh = new BVH(primitives, LEAFSIZE);
    cout << " built!" << endl << "    | " << bvh->nLeafs << " leaves over " << bvh->nNodes << " nodes." << endl;

    /* Close the file. */
    cout << endl << "[+] Scene successfully loaded!" << endl << endl;
    file.close();
}

void Renderer::TonemapRender(Vector* pixels)
{
    /* Sum up the luminance logarithms over the whole render... */
    float avgLuminance = 0.0f;
    for (size_t t = 0; t < pixelCount; ++t)
    {
        /* Compute the luminance and add its logarithm to the sum. */
        float luminance = Luminance(pixels[t], colorSystem);
        avgLuminance += log(luminance + EPSILON);
    }

    /* Get the log-average luminance. */
    avgLuminance = exp(avgLuminance / pixelCount);

    /* Compute the tonemapping key using an exposure of 0.18. */
    float key = 0.18f / avgLuminance;

    /* Go over each pixel... */
    for (size_t t = 0; t < pixelCount; ++t)
    {
        /* Apply the Reinhard operator. */
        float luminance = Luminance(pixels[t], colorSystem);
        pixels[t] = pixels[t] * (key / (1.0f + luminance * key));
    }
}

void Renderer::GammaCorrectRender(Vector* pixels)
{
    /* Apply the gamma correction operator on each pixel. */
    for (size_t t = 0; t < pixelCount; ++t) pixels[t] = GammaCorrect(pixels[t], colorSystem);
}

void Renderer::SaveToPPM(Vector* pixels, string render, time_t elapsedTime)
{
    /* Create the destination file. */
    FILE* file = fopen(render.c_str(), "w");
    if (file == 0) return;

    /* Write a short header. */
    fprintf(file, "P3\n\n# Generated by Lambda.\n# Rendered in %dh%dm%ds.\n\n%d %d 255\n",
            (int)elapsedTime / 3600, (int)(elapsedTime % 3600) / 60, (int)elapsedTime % 60,
            renderParams.width, renderParams.height);

    /* Write the pixel buffer in. */
    for (size_t t = 0; t < pixelCount; ++t)
        fprintf(file, "%d %d %d ", (int)(min(pixels[t].x, 1.0f) * 255.0f),
                                   (int)(min(pixels[t].y, 1.0f) * 255.0f),
                                   (int)(min(pixels[t].z, 1.0f) * 255.0f));

    /* Close the file. */
    fclose(file);
}

float Renderer::Radiance(Ray ray, float wavelength, mt19937* prng)
{
    /* Light path loop. */
    while (true)
    {
        /* Intersect the ray with the scene. */
        Intersection intersection;
        if (!bvh->getIntersection(ray, &intersection, false)) return 0.0f;

        /* Move the ray forward to the intersection point. */
        Vector point = ray.o + ray.d * intersection.t;
        Vector incident = ray.d;

        /* Get the surface normal at the intersection point. */
        Vector normal = intersection.primitive->Normal(point);

        /* If the geometry intersected is a light source, return the emitted light. */
        if (intersection.primitive->light)
        {
            /* Note we assume light sources do not reflect light, this is usually correct. */
            return intersection.primitive->light->Emittance(incident, normal, wavelength);
        }

        /* Otherwise, compute the incoming radiance using the Rendering Equation. To do this elegantly, we
         * compute an importance-sampled ray, then calculate the correct reflectance (if the importance
         * sampling was perfect, the reflectance would be constant, but this is not required). Note the
         * cosine term from Lambert's cosine law is folded into the Reflectance method for efficiency. */
        Vector exitant = intersection.primitive->material->Sample(&point, incident, normal, wavelength, prng);
        float radiance = intersection.primitive->material->Reflectance(incident, exitant, normal, wavelength, true);

        /* Apply the Beer-Lambert Law to attenuate the radiance as the ray travels through the medium. We just find
         * which medium the light ray is actually in, by comparing its last direction with the direction of the
         * normal of the object it last intersected, and compute the amount of loss using the extinction coeff. */
        if (incident * normal > 0.0f) radiance *= exp(-intersection.t * intersection.primitive->material->e2);
        else radiance *= exp(-intersection.t * intersection.primitive->material->e1);

        /* Russian roulette for unbiased depth. Note this means the loop is guaranteed to terminate, since the
         * reflectance is defined as being strictly less than 1. */
        if (RandomVariable(prng) > radiance) return 0.0f;

        /* Go to the next ray bounce. */
        ray = Ray(point, normalize(exitant));
    }

    /* No light path formed. */
    return 0.0f;
}

void Renderer::Render(string render, size_t threads)
{
    /* First, we need to allocate a large enough pixel buffer. */
    Vector* pixels = new Vector[pixelCount];

    /* Set the number of OpenMP threads. If zero was passed, default to the number
     * of execution units available on the system for maximum performance. */
    omp_set_num_threads((threads == 0) ? omp_get_num_procs() : threads);
    cout << "[+] Initializing, " << threads << " threads scheduled..." << flush;

    /* Now, create PRNG states for each thread. Note the seeds are actually
     * deterministic to thread count, which can often be an advantage. */
    vector<mt19937*>* threadPRNG = new vector<mt19937*>();
    for (size_t t = 0; t < threads; ++t)
    {
        mt19937* prng = new mt19937();
        prng->seed(0x530FD819 * (t + 1));
        threadPRNG->push_back(prng);
    }

    /* We're all set, record the starting time. */
    time_t startTime = time(nullptr);
    cout << " ready!" << endl;
    cout << "[+] Raytracing..." << flush;
    time_t lastTime = startTime;
    size_t lastProgress = 0;
    float lastSpeed = 0.0f;
    size_t progress = 0;

    /* Go over each pixel in the image, in parallel. */
    #pragma omp parallel for schedule(dynamic, 1)
    for (size_t t = 0; t < pixelCount; ++t)
    {
        /* Get the current thread's PRNG state. */
        mt19937* prng = threadPRNG->at(threadID);

        /* Get this pixel's position. */
        int x = t % renderParams.width;
        int y = t / renderParams.width;

        /* Create a spectral radiance array. */
        float radiance[WAVELENGTHS] = {0.0f};

        /* Iterate for the number of desired samples... */
        for (int s = 0; s < renderParams.samples; ++s)
        {
            /* Normalize the pixel's coordinates with jitter. */
            float u = 2.0f * ((float)x + RandomVariable(prng) - 0.5f) / renderParams.width - 1.0f;
            float v = 2.0f * ((float)y + RandomVariable(prng) - 0.5f) / renderParams.height - 1.0f;

            /* Multiply the u-coordinate by the aspect ratio. */
            u *= (float)renderParams.width / (float)renderParams.height;

            /* Get a camera ray. */
            Ray ray = camera->Trace(u, v);

            /* Go over each wavelength. */
            for (int w = 0; w < WAVELENGTHS; ++w)
            {
                /* Get a radiance sample for this wavelength. */
                float wavelength = 380.0f + RESOLUTION * w;
                radiance[w] += Radiance(ray, wavelength, prng);
            }
        }

        /* Convert the spectral radiance distribution to an RGB color. */
        pixels[t] = SpectrumToRGB(radiance, colorSystem) / (renderParams.samples * WAVELENGTHS);

        /* We display progress here, so we really only want one thread at a time. */
        #pragma omp critical
        {
            /* This is a shared variable indicating how many pixels
             * of the render have been completed so far. */
            ++progress;

            /* Update every second, if at least some progress has been done. */
            if ((progress > 0) && ((float)difftime(time(nullptr), lastTime) >= 1.0f))
            {
                /* Find the current progress, as a fraction. */
                float completion = (progress - 1) / (float)pixelCount;

                /* Average the render speed with exponential smoothing (alpha = 0.8 works well). */
                float speed = (float)(progress - lastProgress) / difftime(time(nullptr), lastTime);
                if (lastProgress > 0) speed = 0.8f * lastSpeed + 0.2f * speed;

                /* Compute the estimated completion time. */
                int remaining = (int)((pixelCount - progress) / speed);

                /* Display the current progress. */
                printf("\r[+] Raytracing... %04.1f%% [ETC %.3dh%.2dm%.2ds]", completion * 100.0f,
                       remaining / 3600, (remaining % 3600) / 60, remaining % 60);

                /* Reset the last time and flush the console. */
                lastTime = time(nullptr);
                lastProgress = progress;
                lastSpeed = speed;
                cout << flush;
            }
        }
    }

    /* Tonemap, and then gamma-correct the render. */
    TonemapRender(pixels);
    GammaCorrectRender(pixels);

    /* We're finished raytracing, display time taken. */
    int elapsedTime = (int)difftime(time(nullptr), startTime);
    printf("\r[+] Raytracing complete, time taken: %.2dh%.2dm%.2ds.\n",
           elapsedTime / 3600, (elapsedTime % 3600) / 60, elapsedTime % 60);

    /* Save the pixel buffer to a PPM file. */
    cout << endl << "[+] Saving final render in <" << render << ">." << endl;
    SaveToPPM(pixels, render, difftime(time(nullptr), startTime));
    cout << endl << "[+] Render finished!" << endl;

    /* We're done, clean up. */
    for (size_t t = 0; t < threads; ++t) delete threadPRNG->at(t);
    delete threadPRNG;
    delete[] pixels;
}

Renderer::~Renderer()
{
    /* Delete everything we used. */
    for (size_t t = 0; t < distributions->size(); ++t) delete distributions->at(t);
    for (size_t t = 0; t < primitives->size(); ++t) delete primitives->at(t);
    for (size_t t = 0; t < materials->size(); ++t) delete materials->at(t);
    for (size_t t = 0; t < lights->size(); ++t) delete lights->at(t);
    delete distributions;
    delete primitives;
    delete materials;
    delete lights;
    delete camera;
    delete bvh;
}

Lambda
==============

Spectral Path Tracer
--------------

<p align="center">
<img src="https://github.com/TomCrypto/Lambda/blob/master/render/ajax_lucy.png?raw=true" alt="Featured Render"/>
</p>

Lambda is a simple spectral path tracer, written in C++. Some parts of the code are still written in C and don't take advantage of C++ features for the moment. This is a work in progress as many features are still missing, but it is quite functional. Please note Lambda uses C++11 libraries, so you will need a compliant toolchain (for instance, GCC 4.6 and later).

## Currently implemented:

- Unidirectional path tracing with russian roulette
- Spectral Distributions

    Blackbody emission spectrum

    Sellmeier refractive index

    "Flat" spectrum (which is usually used for a wavelength-independent spectral response)

    "Peak" spectrum (which is basically a bell curve at some peak wavelength)

- Materials

    Lambertian diffuse (importance-sampled)

    Ideal specular (importance-sampled)

    Ideal refractive (importance-sampled, unfinished)

    Rough refractive (importance-sampled)

    Cook-Torrance (importance-sampled, w/ Beckmann)

- Lights

    Omni light (i.e. isotropic emittance)

- Primitives

    Spheres

    Triangles

- Gamma correction
- Reinhard tone-mapping
- Multiple available color spaces
- Scalable multithreading via OpenMP
- Robust pseudorandom number generation (C++11 mersenne twister)
- Very efficient bounding volume hierarchy acceleration structure (many thanks to [Brandon Pelfrey](https://github.com/brandonpelfrey))

## Missing features

- Bidirectional path tracing
- More of everything else
- ...

## How to build

There is a Code::Blocks project included, but if you don't want to use that, I haven't done a Makefile yet. To compile, you don't need anything special, but if you want to use OpenMP for multithreading, you'll need to link to the "gomp" library and pass the -fopenmp flag to your compiler.

## How to use

Lambda works on the basis of "scene files", which contain everything needed to render a given scene. These need to follow a certain format, which is fairly obvious to work out if you look at the loading code. I provide some sample scene files in the repository, though some of them are necessarily quite large due to the amount of triangles required. You can also create your own scenes, I intend to provide helper functions to ease this task later on.

## Where are the scenes files?

There are some rather generic ones in the scenes/ folder. The other, high-detail ones, because of their large size, are located in the [Downloads](https://github.com/TomCrypto/Lambda/downloads) section of the repository in compressed form (7z).

## Where is the result?

Lambda produces its output in gamma-corrected, tone-mapped PPM format. If you are under Linux, you should be able to view it without issues. Under Windows, you can use any PPM reader, such as IrfanView or other (and you can then convert it to a PNG, for instance).

## Compatibility

Lambda is compatible with C++11 compliant compilers and uses only standard libraries. As such, it is portable, and has been tested under 64-bit Linux and 64-bit Windows. Note the vector code requires SSE3 instructions, but you really should have those.

## Misc.

Note most implementations in this repository are untested when it comes to physically-based correctness. We try to implement things correctly, but it is very difficult to distinguish "correct" from "almost correct" without reference renders, which we unfortunately do not have.

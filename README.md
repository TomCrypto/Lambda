Lambda
==============

Spectral Path Tracer
--------------

<p align="center">
<img src="https://github.com/TomCrypto/Lambda/blob/master/render/frosted_dragon.png?raw=true" alt="Frosted Glass Dragon"/>
</p>

Lambda is a simple spectral path tracer, written in C++. Some parts of the code are still written in C and don't take advantage of C++ features for the moment. This is a work in progress as many features are still missing, but it is quite functional. Please note Lambda uses C++11 libraries, so you will need a compliant toolchain (for instance, GCC 4.6 and later).

## Currently implemented:

- unidirectional path tracing with russian roulette
- various spectral distributions:
  [+] blackbody emission spectrum
  [+] sellmeier refractive index
  [+] "flat" spectrum (which is usually used for a wavelength-independent spectral response)
  [+] "peak" spectrum (which is basically a bell curve at some peak wavelength)
- some materials
  [+] lambertian diffuse (importance-sampled)
  [+] ideal specular (importance-sampled)
  [+] ideal refractive (importance-sampled, unfinished)
  [+] rough refractive (not quite physically correct, unfinished)
- lights
  [+] omni light (i.e. isotropic emittance)
- primitives
  [+] spheres
  [+] triangles
- scalable multithreading via OpenMP
- robust pseudorandom number generation (C++11 mersenne twister)
- very efficient bounding volume hierarchy acceleration structure (many thanks to [Brandon Pelfrey](https://github.com/brandonpelfrey))

## Missing features

- bidirectional path tracing
- more distributions, materials, lights and primitives
- ...

## How to use

Lambda works on the basis of "scene files", which contain everything needed to render a given scene. These need to follow a certain format, which is fairly obvious to work out if you look at the loading code. I provide some sample scene files in the repository, though some of them are necessarily quite large due to the amount of triangles required. You can also create your own scenes, I intend to provide helper functions to ease this task later on.

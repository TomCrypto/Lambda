#ifndef CIE_H
#define CIE_H

#include <util/vec3.hpp>
#include <algorithm>

/* This is the resolution of the color-matching curves, in nanometers per sample. */
#define RESOLUTION 5

/* The number of wavelengths used per pixel sample. */
#define WAVELENGTHS (1 + 400 / RESOLUTION)

/* These are some scene file definitions for color systems. */
#define ID_EBU 0
#define ID_SMPTE 1
#define ID_HDTV 2
#define ID_REC709 3
#define ID_NTSC 4
#define ID_CIE 5

/* This is a color system. */
typedef struct ColorSystem{
    double xRed, yRed;	    	    /* Red x, y */
    double xGreen, yGreen;  	    /* Green x, y */
    double xBlue, yBlue;     	    /* Blue x, y */
    double xWhite, yWhite;  	    /* White point x, y */
	double gamma;   	    	    /* Gamma correction for system */
} ColorSystem;

/* These are some relatively common illuminants (white points). */
#define IlluminantC     0.3101, 0.3162	    	/* For NTSC television */
#define IlluminantD65   0.3127, 0.3291	    	/* For EBU and SMPTE */
#define IlluminantE 	0.33333333, 0.33333333  /* CIE equal-energy illuminant */

/* 0 represents a special gamma function. */
#define GAMMA_REC709 0

/* These are some standard color systems. */
const ColorSystem /* xRed    yRed    xGreen  yGreen  xBlue  yBlue    White point        Gamma   */
    EBUSystem    =  {0.64,   0.33,   0.29,   0.60,   0.15,   0.06,   IlluminantD65,  GAMMA_REC709},
    SMPTESystem  =  {0.630,  0.340,  0.310,  0.595,  0.155,  0.070,  IlluminantD65,  GAMMA_REC709},
    HDTVSystem   =  {0.670,  0.330,  0.210,  0.710,  0.150,  0.060,  IlluminantD65,  GAMMA_REC709},
    Rec709System =  {0.64,   0.33,   0.30,   0.60,   0.15,   0.06,   IlluminantD65,  GAMMA_REC709},
    NTSCSystem   =  {0.67,   0.33,   0.21,   0.71,   0.14,   0.08,   IlluminantC,    GAMMA_REC709},
    CIESystem    =  {0.7355, 0.2645, 0.2658, 0.7243, 0.1669, 0.0085, IlluminantE,    GAMMA_REC709};

/* A static array of standard color systems. */
const ColorSystem ColorSystems[6] = {EBUSystem, SMPTESystem, HDTVSystem, Rec709System, NTSCSystem, CIESystem};

/* Converts a spectral radiance distribution to an RGB color. */
Vector SpectrumToRGB(float spectralRadiance[WAVELENGTHS], ColorSystem colorSystem);

/* Returns the luminance of an RGB color according to a given color system. */
float Luminance(Vector rgb, ColorSystem colorSystem);

/* Gamma corrects an RGB color according to a given color system. */
Vector GammaCorrect(Vector rgb, ColorSystem colorSystem);

#endif

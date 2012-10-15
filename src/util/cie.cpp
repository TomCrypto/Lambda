#include <util/cie.hpp>

/* The CIE color matching curve, at 5nm intervals. */
static Vector ColorMatchingCurve[81] = {
    Vector(0.0014,0.0000,0.0065), Vector(0.0022,0.0001,0.0105), Vector(0.0042,0.0001,0.0201),
    Vector(0.0076,0.0002,0.0362), Vector(0.0143,0.0004,0.0679), Vector(0.0232,0.0006,0.1102),
    Vector(0.0435,0.0012,0.2074), Vector(0.0776,0.0022,0.3713), Vector(0.1344,0.0040,0.6456),
    Vector(0.2148,0.0073,1.0391), Vector(0.2839,0.0116,1.3856), Vector(0.3285,0.0168,1.6230),
    Vector(0.3483,0.0230,1.7471), Vector(0.3481,0.0298,1.7826), Vector(0.3362,0.0380,1.7721),
    Vector(0.3187,0.0480,1.7441), Vector(0.2908,0.0600,1.6692), Vector(0.2511,0.0739,1.5281),
    Vector(0.1954,0.0910,1.2876), Vector(0.1421,0.1126,1.0419), Vector(0.0956,0.1390,0.8130),
    Vector(0.0580,0.1693,0.6162), Vector(0.0320,0.2080,0.4652), Vector(0.0147,0.2586,0.3533),
    Vector(0.0049,0.3230,0.2720), Vector(0.0024,0.4073,0.2123), Vector(0.0093,0.5030,0.1582),
    Vector(0.0291,0.6082,0.1117), Vector(0.0633,0.7100,0.0782), Vector(0.1096,0.7932,0.0573),
    Vector(0.1655,0.8620,0.0422), Vector(0.2257,0.9149,0.0298), Vector(0.2904,0.9540,0.0203),
    Vector(0.3597,0.9803,0.0134), Vector(0.4334,0.9950,0.0087), Vector(0.5121,1.0000,0.0057),
    Vector(0.5945,0.9950,0.0039), Vector(0.6784,0.9786,0.0027), Vector(0.7621,0.9520,0.0021),
    Vector(0.8425,0.9154,0.0018), Vector(0.9163,0.8700,0.0017), Vector(0.9786,0.8163,0.0014),
    Vector(1.0263,0.7570,0.0011), Vector(1.0567,0.6949,0.0010), Vector(1.0622,0.6310,0.0008),
    Vector(1.0456,0.5668,0.0006), Vector(1.0026,0.5030,0.0003), Vector(0.9384,0.4412,0.0002),
    Vector(0.8544,0.3810,0.0002), Vector(0.7514,0.3210,0.0001), Vector(0.6424,0.2650,0.0000),
    Vector(0.5419,0.2170,0.0000), Vector(0.4479,0.1750,0.0000), Vector(0.3608,0.1382,0.0000),
    Vector(0.2835,0.1070,0.0000), Vector(0.2187,0.0816,0.0000), Vector(0.1649,0.0610,0.0000),
    Vector(0.1212,0.0446,0.0000), Vector(0.0874,0.0320,0.0000), Vector(0.0636,0.0232,0.0000),
    Vector(0.0468,0.0170,0.0000), Vector(0.0329,0.0119,0.0000), Vector(0.0227,0.0082,0.0000),
    Vector(0.0158,0.0057,0.0000), Vector(0.0114,0.0041,0.0000), Vector(0.0081,0.0029,0.0000),
    Vector(0.0058,0.0021,0.0000), Vector(0.0041,0.0015,0.0000), Vector(0.0029,0.0010,0.0000),
    Vector(0.0020,0.0007,0.0000), Vector(0.0014,0.0005,0.0000), Vector(0.0010,0.0004,0.0000),
    Vector(0.0007,0.0002,0.0000), Vector(0.0005,0.0002,0.0000), Vector(0.0003,0.0001,0.0000),
    Vector(0.0002,0.0001,0.0000), Vector(0.0002,0.0001,0.0000), Vector(0.0001,0.0000,0.0000),
    Vector(0.0001,0.0000,0.0000), Vector(0.0001,0.0000,0.0000), Vector(0.0000,0.0000,0.0000)};

/* Converts a spectral radiance distribution to an RGB color. */
Vector SpectrumToRGB(float spectralRadiance[WAVELENGTHS], ColorSystem colorSystem)
{
    /* Simply integrate the color-matching curve. */
    float radiance = 0;
    Vector color = Vector(0, 0, 0);
    for (int w = 0; w < WAVELENGTHS; w++)
    {
        color = color + ColorMatchingCurve[w * (RESOLUTION / 5)] * spectralRadiance[w];
        radiance += spectralRadiance[w];
    }

    /* Normalize the XYZ color. */
    float sum = color.x + color.y + color.z;
    if (sum > EPSILON) color = color / sum;

    /* Decode the color system. */
    float xr = colorSystem.xRed;   float yr = colorSystem.yRed;   float zr = 1 - (xr + yr);
    float xg = colorSystem.xGreen; float yg = colorSystem.yGreen; float zg = 1 - (xg + yg);
    float xb = colorSystem.xBlue;  float yb = colorSystem.yBlue;  float zb = 1 - (xb + yb);
    float xw = colorSystem.xWhite; float yw = colorSystem.yWhite; float zw = 1 - (xw + yw);

    /* Compute the XYZ to RGB matrix. */
    float rx = (yg * zb) - (yb * zg);
    float ry = (xb * zg) - (xg * zb);
    float rz = (xg * yb) - (xb * yg);
    float gx = (yb * zr) - (yr * zb);
    float gy = (xr * zb) - (xb * zr);
    float gz = (xb * yr) - (xr * yb);
    float bx = (yr * zg) - (yg * zr);
    float by = (xg * zr) - (xr * zg);
    float bz = (xr * yg) - (xg * yr);

    /* Compute the RGB luminance scaling factor. */
    float rw = ((rx * xw) + (ry * yw) + (rz * zw)) / yw;
    float gw = ((gx * xw) + (gy * yw) + (gz * zw)) / yw;
    float bw = ((bx * xw) + (by * yw) + (bz * zw)) / yw;

    /* Scale the XYZ to RGB matrix to white. */
    rx = rx / rw;  ry = ry / rw;  rz = rz / rw;
    gx = gx / gw;  gy = gy / gw;  gz = gz / gw;
    bx = bx / bw;  by = by / bw;  bz = bz / bw;

    /* Calculate the desired RGB. */
    Vector rgb = Vector((rx * color.x) + (ry * color.y) + (rz * color.z),
                        (gx * color.x) + (gy * color.y) + (gz * color.z),
                        (bx * color.x) + (by * color.y) + (bz * color.z));

    /* Constrain the RGB color within the RGB gamut. */
    float w = std::min(0.0f, std::min(rgb.x, std::min(rgb.y, rgb.z)));
    rgb = rgb - Vector(w, w, w);

    /* Multiply the final RGB color by the pixel's radiance. */
    return rgb * radiance;
}

/* Returns the luminance of an RGB color according to a given color system. */
float Luminance(Vector rgb, ColorSystem colorSystem)
{
    return rgb.x * colorSystem.yRed + rgb.y * colorSystem.yGreen + rgb.z * colorSystem.yBlue;
}

/* Gamma corrects an RGB color according to a given color system. */
Vector GammaCorrect(Vector rgb, ColorSystem colorSystem)
{
    /* If we're using the special REC709 gamma formula... */
    if (colorSystem.gamma == GAMMA_REC709)
    {
        /* Use the special piecewise formula. */
        if (rgb.x >= 0.018) rgb.x = (1.099 * pow(rgb.x, 0.45)) - 0.099;
        else rgb.x *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;
        if (rgb.y >= 0.018) rgb.y = (1.099 * pow(rgb.y, 0.45)) - 0.099;
        else rgb.y *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;
        if (rgb.z >= 0.018) rgb.z = (1.099 * pow(rgb.z, 0.45)) - 0.099;
        else rgb.z *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;

        /* Return the gamma-corrected color. */
        return rgb;
    }
    else
    {
        /* Otherwise, use a standard gamma power curve. */
        float power = 1.0f / colorSystem.gamma;
        return Vector(powf(rgb.x, power), powf(rgb.y, power), powf(rgb.z, power));
    }
}

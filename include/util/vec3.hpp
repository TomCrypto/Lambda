#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <util/rtmath.hpp>

/* Single-precision epsilon. */
#define EPSILON 1e-5f

/* Pi. */
#define PI 3.14159265f

/* Handy sign function. */
#define sign(x) ((x > 0.0) - (x < 0.0))

/* Zero vector. */
#define ZERO Vector(0.0f, 0.0f, 0.0f)

/* Conventional upwards vector. */
#define UPWARDS Vector(0.0f, 1.0f, 0.0f)



// SSE Vector object
struct Vector {
	union __attribute__((aligned(16))) {
		struct { float x,y,z,w; };
		__m128 m128;
	};

	Vector() { }
	Vector(float x, float y, float z, float w=0.f) : m128(_mm_set_ps(w,z,y,x)) { }
	Vector(const __m128& m128) : m128(m128) { }

    void operator+=(Vector a) { *this = *this + a; }
    void operator-=(Vector a) { *this = *this - a; }
    void operator*=(float a) { *this = *this * a; }

	Vector operator+(const Vector& b) const { return _mm_add_ps(m128, b.m128); }
	Vector operator-(const Vector& b) const { return _mm_sub_ps(m128, b.m128); }
	Vector operator*(float b) const { return _mm_mul_ps(m128, _mm_set_ps(b,b,b,b)); }
	Vector operator/(float b) const { return _mm_div_ps(m128, _mm_set_ps(b,b,b,b)); }

 // Component-wise multiply and divide
 Vector cmul(const Vector& b) const { return _mm_mul_ps(m128, b.m128); }
 Vector cdiv(const Vector& b) const { return _mm_div_ps(m128, b.m128); }

	// dot (inner) product
	float operator*(const Vector& b) const {
		return x*b.x + y*b.y + z*b.z;
	}

 // Cross Product
	Vector operator^(const Vector& b) const {
		return _mm_sub_ps(
		_mm_mul_ps(
			_mm_shuffle_ps(m128, m128, _MM_SHUFFLE(3, 0, 2, 1)),
			_mm_shuffle_ps(b.m128, b.m128, _MM_SHUFFLE(3, 1, 0, 2))),
		_mm_mul_ps(
			_mm_shuffle_ps(m128, m128, _MM_SHUFFLE(3, 1, 0, 2)),
			_mm_shuffle_ps(b.m128, b.m128, _MM_SHUFFLE(3, 0, 2, 1)))
		);
	}

	Vector operator/(const Vector& b) const { return _mm_div_ps(m128, b.m128); }

 // Handy component indexing
 float& operator[](const unsigned int i) { return (&x)[i]; }
	const float& operator[](const unsigned int i) const { return (&x)[i]; }
};

inline Vector operator*(float a, const Vector&b)  { return _mm_mul_ps(_mm_set1_ps(a), b.m128); }

// Component-wise min
inline Vector min(const Vector& a, const Vector& b) {
	return _mm_min_ps(a.m128, b.m128);
}

// Component-wise max
inline Vector max(const Vector& a, const Vector& b) {
	return _mm_max_ps(a.m128, b.m128);
}

// Length of a vector
inline float length(const Vector& a) {
	return sqrtf(a*a);
}

// Make a vector unit length
inline Vector normalize(const Vector& in) {
	Vector a = in;
	a.w = 0.f;

	__m128 D = a.m128;
 D = _mm_mul_ps(D, D);
	D = _mm_hadd_ps(D, D);
	D = _mm_hadd_ps(D, D);

	// 1 iteration of Newton-raphson -- Idea from Intel's Embree.
	__m128 r = _mm_rsqrt_ps(D);
 r = _mm_add_ps(
  _mm_mul_ps(_mm_set_ps(1.5f, 1.5f, 1.5f, 1.5f), r),
  _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(D, _mm_set_ps(-0.5f, -0.5f, -0.5f, -0.5f)), r), _mm_mul_ps(r, r)));

 return _mm_mul_ps( a.m128, r );
}

inline Vector lerp(Vector a, Vector b, float t)
{
    return a + (b - a) * t;
}

/* Returns a unit vector from a pair of spherical coordinates, azimuth and inclination. */
inline Vector spherical(float phi, float theta)
{
    return Vector{cosf(phi) * sinf(theta),  cosf(theta), sinf(phi) * sinf(theta)};
}

/* Rotates a unit vector around a normal vector - more accurately, transforms the vector from world coordinates to
 * normal coordinates defined by the orthonormal basis described by the normal vector as the upwards axis. */
inline Vector rotate(Vector a, Vector n)
{
    /* If the normal vector is already the world space upwards (or downwards) vector, don't do anything. */
    if (!delta(1 - std::abs(n * UPWARDS)))
    {
        /* Build the orthonormal basis of the normal vector. */
        Vector bX = normalize(n ^ UPWARDS);
        Vector bZ = normalize(n ^ bX);

        /* Transform the unit vector to this basis. */
        return bX * a.x + n * a.y + bZ * a.z;
    } else return a * sign(a * n);
}

/* Returns the reflection of a vector on a normal. */
inline Vector reflect(Vector i, Vector n)
{
    return i - n * (2 * (i * n));
}

/* This is a ray. */
struct Ray {
 Vector o; // Ray Origin
 Vector d; // Ray Direction
 Vector inv_d; // Inverse of each Ray Direction component

 Ray(const Vector& o, const Vector& d)
 : o(o), d(d), inv_d(Vector(1,1,1).cdiv(d)) { }
};

#endif

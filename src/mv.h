// ============================================================================
// File:        mv.h
// Description: Matrix and vector math header
// Author:      Shirobon
// Date:        2023/12/04
// ============================================================================

#ifndef MV_H
#define MV_H

#include "config.h"

#include <stdlib.h>
#include <math.h>

// Useful constants
#define MV_PI           (3.14159265359f)
#define MV_2PI          (6.28318530718f)
#define MV_PIDIV2       (1.57079632679f)
#define MV_PIDIV4       (0.78539816340f)
#define MV_PIDIV180     (0.01745329252f)
#define MV_PIDIV180I    (57.2957795131f) // Inverse of pi/180

#define MV_DEG_TO_RAD(x) ((x) * MV_PIDIV180)
#define MV_RAD_TO_DEG(x) ((x) * MV_PIDIV180I)

// ============================================================================
// Type Definitions
// ============================================================================

typedef struct vec3_t
{
    float x, y, z;
} vec3_t;

typedef struct mat4_t
{
    float e[16];
} mat4_t;

// ============================================================================
// Vector Operations
// ============================================================================

INLINE vec3_t mv_vec_new(float x, float y, float z)
{
    return (vec3_t){x, y, z};
}

INLINE vec3_t mv_vec_add(vec3_t a, vec3_t b)
{
    return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}
INLINE vec3_t mv_vec_sub(vec3_t a, vec3_t b)
{
    return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

INLINE vec3_t mv_vec_scale(vec3_t a, float factor)
{
    return (vec3_t){a.x * factor, a.y * factor, a.z * factor};
}

INLINE float mv_vec_scalar_product(vec3_t a, vec3_t b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

INLINE vec3_t mv_vec_cross_product(vec3_t a, vec3_t b)
{
    return (vec3_t){( a.y * b.z) - (b.y * a.z),
                    (-a.x * b.z) + (b.x * a.z),
                    ( a.x * b.y) - (b.x * a.y)};
}

INLINE float mv_vec_length(vec3_t a)
{
    return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

INLINE vec3_t mv_vec_normalize(vec3_t a)
{
    float length = mv_vec_length(a);
    return (vec3_t){a.x / length, a.y / length, a.z / length};
}

INLINE float mv_vec_distance(vec3_t a, vec3_t b)
{
    return mv_vec_length(mv_vec_sub(a, b));
}

INLINE vec3_t mv_unit_normal_from_plane(vec3_t a, vec3_t b, vec3_t c)
{
    vec3_t ab = mv_vec_sub(b, a);
    vec3_t ac = mv_vec_sub(c, a);
    return mv_vec_normalize(mv_vec_cross_product(ab, ac));
}

// ============================================================================
// Matrix Operations
// ============================================================================

INLINE mat4_t mv_mat_multiply(mat4_t a, mat4_t b)
{
    mat4_t result;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.e[(j*4)+1] = (a.e[0+i]  * b.e[(j*4)])    + \
                                (a.e[4+i]  * b.e[(j*4)+1])  + \
                                (a.e[8+i]  * b.e[(j*4)+2])  + \
                                (a.e[12+i] * b.e[(j*4)+3]);
        }
    }

    return result;
}

INLINE mat4_t mv_mat_identity_matrix(void)
{
    // It may look like row major, but it's the same order as column major
    return (mat4_t) { { 1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f } };
}

INLINE mat4_t mv_mat_scale_matrix(float x, float y, float z)
{
    // It may look like row major, but it's the same order as column major
    return (mat4_t) { { x   , 0.0f, 0.0f, 0.0f,
                        0.0f, y   , 0.0f, 0.0f,
                        0.0f, 0.0f, z   , 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f } };
}

INLINE mat4_t mv_mat_translation_matrix(float x, float y, float z)
{
    return (mat4_t) { { 1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                           x,    y,    z, 1.0f } };
}

INLINE mat4_t mv_mat_rotation_matrix(float angle, float x, float y, float z)
{
    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;

    vec3_t axis = mv_vec_normalize((vec3_t){x, y, z});

    x = axis.x;
    y = axis.y;
    z = axis.z;

    mat4_t r;

    r.e[0] = (x*x*t)+c;     r.e[4] = (t*x*y)-(s*z); r.e[8]  = (t*x*z)+(s*y); r.e[12] = 0.0f;
    r.e[1] = (t*x*y)+(s*z); r.e[5] = (t*y*y)+c;     r.e[9]  = (t*y*z)-(s*x); r.e[13] = 0.0f;
    r.e[2] = (t*x*z)-(s*y); r.e[6] = (t*y*z)+(s*x); r.e[10] = (t*z*z)+c;     r.e[14] = 0.0f;
    r.e[3] = 0.0f;          r.e[7] = 0.0f;          r.e[11] = 0.0f;          r.e[15] = 1.0f;

    return r;
}

INLINE mat4_t mv_mat_orthographic_matrix(float l, float r, float b, float t, float zn, float zf)
{
    mat4_t m =  {{0.0f}};
    m.e[0]   =  2.0f/(r-l);
    m.e[5]   =  2.0f/(t-b);
    m.e[10]  = -2.0f/(zf-zn);
    m.e[12]  = -(r+l)/(r-l);
    m.e[13]  = -(t+b)/(t-b);
    m.e[14]  = -(zf+zn)/(zf-zn);
    m.e[15]  =  1.0f;

    return m;
}

INLINE mat4_t mv_mat_perspective_matrix_impl(float l, float r, float b, float t, float zn, float zf)
{
    mat4_t m =  {{0.0f}};
    m.e[0]   =  (2.0f*zn)/(r-l);
    m.e[5]   =  (2.0f*zn)/(t-b);
    m.e[8]   =  (r+l)/(r-l);
    m.e[9]   =  (t+b)/(t-b);
    m.e[10]  = -(zf+zn)/(zf-zn);
    m.e[11]  = -1.0f;
    m.e[14]  = -(2.0f*zf*zn)/(zf-zn);

    return m;
}

INLINE mat4_t mv_mat_perspective_matrix(float fov, float aspect, float zn, float zf)
{
    float y = zn * tanf(fov/2.0f);
    float x = y  * aspect;
    return mv_mat_perspective_matrix_impl(-x, x, -y, y, zn, zf);
}

INLINE vec3_t mv_mat_vec_transform(mat4_t m, vec3_t v)
{
    float x = m.e[0]*v.x + m.e[4]*v.y + m.e[8] *v.z + m.e[12]*1.0f;
    float y = m.e[1]*v.x + m.e[5]*v.y + m.e[9] *v.z + m.e[13]*1.0f;
    float z = m.e[2]*v.x + m.e[6]*v.y + m.e[10]*v.z + m.e[14]*1.0f;
    float w = m.e[3]*v.x + m.e[7]*v.y + m.e[11]*v.z + m.e[15]*1.0f;

    return (vec3_t){x/w, y/w, z/w}; // Do perspective divide here
}



#endif // MV_H

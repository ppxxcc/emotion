// ============================================================================
// File:        mv.h
// Description: Matrix and Vectors (header)
// Author:      Shirobon
// Date:        2023/12/04
// ============================================================================

#ifndef MV_H
#define MV_H

#include "config.h"

#include "debug.h"

#include <stdlib.h>
#include <dc/fmath.h>

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

typedef enum mv_matrix_model_t {
    MV_MODELVIEW,
    MV_PROJECTION
} mv_matrix_model_t;



// ============================================================================
// Function Declarations
// ============================================================================

void mv_set_matrix_model(mv_matrix_model_t m);
void mv_push_matrix(void);
void mv_pop_matrix(void);
void mv_identity(void);
void mv_rotate(float angle, float x, float y, float z);
void mv_scale(float x, float y, float z);
void mv_translate(float x, float y, float z);
void mv_ortho(float left, float right, float bottom, float top, float near, float far);
void mv_frustum(float left, float right, float bottom, float top, float near, float far);
void mv_perspective(float fovy, float aspect, float near, float far);
void mv_calculate_transform(void);

mat4_t mv_get_matrix(mv_matrix_model_t m);

void mv_print_matrix(mat4_t m);

// ============================================================================
// Global Transformation Matrix - Matrix to apply when transforming vertices
// ============================================================================
extern mat4_t g_mv_transform;

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
    return fsqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
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

INLINE vec3_t mv_mat_vec_transform(vec3_t v)
{
    #define m g_mv_transform

    float x = m.e[0]*v.x + m.e[4]*v.y + m.e[8] *v.z + m.e[12]*1.0f;
    float y = m.e[1]*v.x + m.e[5]*v.y + m.e[9] *v.z + m.e[13]*1.0f;
    float z = m.e[2]*v.x + m.e[6]*v.y + m.e[10]*v.z + m.e[14]*1.0f;
    float w = m.e[3]*v.x + m.e[7]*v.y + m.e[11]*v.z + m.e[15]*1.0f;

    #undef m

    return (vec3_t){x/w, y/w, z/w}; // Do perspective divide here
}

#endif // MV_H

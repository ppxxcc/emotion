// ============================================================================
// File:        mv.h
// Description: Matrix and Vectors (source)
// Author:      Shirobon
// Date:        2023/12/08
// ============================================================================

#include "config.h"

#include "debug.h"

#include "mv.h"

#include <stdint.h>

// Global so it can be directly accessed from inlined function
// Since the transform will need to be applied to every vertex
mat4_t g_mv_transform;

static mat4_t modelview;
static mat4_t projection;

static mat4_t modelview_stack[CONFIG_MATRIX_STACK_SIZE];
static mat4_t projection_stack[CONFIG_MATRIX_STACK_SIZE];

static size_t modelview_top;
static size_t projection_top;

static mv_matrix_model_t model;

// ============================================================================
// Internal Inline Matrix Operations
// ============================================================================

#define ASSIGN_TO_ACTIVE_MATRIX(m) \
    if(model == MV_MODELVIEW) {    \
        modelview = m;             \
    } else {                       \
        projection = m;            \
    }                              \

#define ASSIGN_ACTIVE_MATRIX()    \
    (model == MV_MODELVIEW ? modelview : projection)

INLINE mat4_t matrix_multiply(mat4_t a, mat4_t b)
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

// ============================================================================
// Matrix Stack Operations
// ============================================================================
void mv_set_matrix_model(mv_matrix_model_t m)
{
    model = m;
}

void mv_push_matrix(void)
{
    if(model == MV_MODELVIEW) {
        modelview_top++;
        if(modelview_top > CONFIG_MATRIX_STACK_SIZE) {
            modelview_top--;
            debug_printf(DEBUG_ERROR, "Cannot push to full modelview matrix stack.\n");
            return;
        }
        modelview_stack[CONFIG_MATRIX_STACK_SIZE - modelview_top] = modelview;
    }

    else { // MV_PROJECTION
        projection_top++;
        if(projection_top > CONFIG_MATRIX_STACK_SIZE) {
            projection_top--;
            debug_printf(DEBUG_ERROR, "Cannot push to full projection matrix stack.\n");
            return;
        }
        projection_stack[CONFIG_MATRIX_STACK_SIZE - projection_top] = projection;
    }
}

void mv_pop_matrix(void)
{
    if(model == MV_MODELVIEW) {
        if(modelview_top == 0) {
            debug_printf(DEBUG_ERROR, "Cannot pop from empty modelview matrix stack.\n");
            return;
        }
        modelview = modelview_stack[CONFIG_MATRIX_STACK_SIZE - modelview_top];
        modelview_top--;
    }

    else { // MV_PROJECTION
        if(projection_top == 0) {
            debug_printf(DEBUG_ERROR, "Cannot pop from empty projection matrix stack.\n");
            return;
        }
        projection = projection_stack[CONFIG_MATRIX_STACK_SIZE - projection_top];
        projection_top--;
    }    
}


// ============================================================================
// Matrix Transform Operations
// ============================================================================
void mv_identity(void)
{
    // It may look like row major, but it's the same order as column major
    mat4_t m = { { 1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f } };
    
    ASSIGN_TO_ACTIVE_MATRIX(m);
}

void mv_rotate(float angle, float x, float y, float z)
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

    mat4_t m = ASSIGN_ACTIVE_MATRIX();

    m = matrix_multiply(m, r);

    ASSIGN_TO_ACTIVE_MATRIX(m);
}

void mv_scale(float x, float y, float z)
{
    // It may look like row major, but it's the same order as column major
    mat4_t s = { { x   , 0.0f, 0.0f, 0.0f,
                   0.0f, y   , 0.0f, 0.0f,
                   0.0f, 0.0f, z   , 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f } };
    
    mat4_t m = ASSIGN_ACTIVE_MATRIX();

    m = matrix_multiply(m, s);

    ASSIGN_TO_ACTIVE_MATRIX(m);
}

void mv_translate(float x, float y, float z)
{
    mat4_t m = ASSIGN_ACTIVE_MATRIX();

    m.e[12] += x;
    m.e[13] += y;
    m.e[14] += z;

    ASSIGN_TO_ACTIVE_MATRIX(m);
}

void mv_ortho(float left, float right, float bottom, float top, float near, float far)
{
    mat4_t o =  {{0.0f}};
    o.e[0]   =  2.0f/(right-left);
    o.e[5]   =  2.0f/(top-bottom);
    o.e[10]  = -2.0f/(far-near);
    o.e[12]  = -(right+left)/(right-left);
    o.e[13]  = -(top+bottom)/(top-bottom);
    o.e[14]  = -(far+near)/(far-near);
    o.e[15]  =  1.0f;

    mat4_t m = ASSIGN_ACTIVE_MATRIX();

    m = matrix_multiply(m, o);

    ASSIGN_TO_ACTIVE_MATRIX(m);
}

void mv_frustum(float left, float right, float bottom, float top, float near, float far)
{
    mat4_t f =  {{0.0f}};
    f.e[0]   =  (2.0f*near)/(right-left);
    f.e[5]   =  (2.0f*near)/(top-bottom);
    f.e[8]   =  (right+left)/(right-left);
    f.e[9]   =  (top+bottom)/(top-bottom);
    f.e[10]  = -(far+near)/(far-near);
    f.e[11]  = -1.0f;
    f.e[14]  = -(2.0f*far*near)/(far-near);

    mat4_t m = ASSIGN_ACTIVE_MATRIX();

    m = matrix_multiply(m, f);

    ASSIGN_TO_ACTIVE_MATRIX(m);   
}

void mv_perspective(float fovy, float aspect, float near, float far)
{
    float x, y;
    y = near * tanf(fovy / 2.0f);
    x = y * aspect;
    mv_frustum(-x, x, -y, y, near, far);
}

void mv_calculate_transform(void)
{
    g_mv_transform = matrix_multiply(projection, modelview);
}
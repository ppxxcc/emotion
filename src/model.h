// ============================================================================
// File:        model.h
// Description: 3D model related functionality (header)
// Author:      Shirobon
// Date:        2023/12/05
// ============================================================================

#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>

#include "graphics.h"

typedef uint8_t model_mid_t;

#define MODEL_ERROR (255)
#define MODEL_FREED (254)

typedef struct model_face_t
{
    gfx_vertex_t a, b, c;
} model_face_t;

typedef struct model_t
{
    model_face_t* faces;
    gfx_tid_t     tid;
    bool          textured;
    model_mid_t   mid;
} model_t;

model_mid_t model_load_obj(void* obj_asset, gfx_tid_t tid, bool textured);


#endif

// ============================================================================
// File:        model.h
// Description: 3D model related functionality (source)
// Author:      Shirobon
// Date:        2023/12/05
// ============================================================================

#include "model.h"

#include <ctype.h>
#include <string.h>

static model_t* models; // Array type of active models

static size_t model_id_count; // These bss symbols are already initialized to 0.
static size_t model_active_count;
static size_t model_memory;

model_mid_t model_load_obj(const char* asset, gfx_tid_t tid, bool textured)
{
    model_t model;
    model.faces    = NULL;
    model.tid      = textured ? tid : GFX_UNUSED;
    model.textured = textured;

    FILE* f = NULL;
    
    size_t vertex_count = 0;
    size_t uv_count     = 0;
    size_t face_count   = 0;
    
    gfx_vertex_t* vertices = NULL;
    float*        uv       = NULL;
    model_face_t* faces    = NULL;
    

    char line[256] = {0}; // 255 characters for a line should be enough.

    if((f = fopen(asset, "r")) == NULL) {
        printf("Couldn't open specified model: %s\n", asset);
        return MODEL_ERROR;
    }

    // Initially parse for amount of vertices, UVs and faces
    while(fgets(line, sizeof(line), f)) {
        if(line[0] == 'v' && isspace(line[1])) {
            vertex_count++;
        }
        else if(line[0] == 'v' && line[1] == 't') {
            uv_count++;
        }
        else if(line[0] == 'f') {
            face_count++;
        }
    }

}
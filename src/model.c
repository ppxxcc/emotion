// ============================================================================
// File:        model.h
// Description: 3D model related functionality (source)
// Author:      Shirobon
// Date:        2023/12/05
// ============================================================================

#include "model.h"

#include <string.h>

//static model_t* models; // Array type of active models

//static size_t model_id_count; // These bss symbols are already initialized to 0.
//static size_t model_active_count;
//static size_t model_memory;

/*
model_mid_t model_load_obj(void* asset, gfx_tid_t tid, bool textured)
{
    char* obj = asset;

    model_t model;
    model.faces    = NULL;
    model.tid      = textured ? tid : GFX_UNUSED;
    model.textured = textured;

    


    return 0;
}*/
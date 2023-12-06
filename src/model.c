// ============================================================================
// File:        model.h
// Description: 3D model related functionality (source)
// Author:      Shirobon
// Date:        2023/12/05
// ============================================================================

#include "config.h"
#include "model.h"
#include "debug.h"

#include <ctype.h>
#include <string.h>

static model_t* models; // Array type of active models

static size_t model_id_count; // These bss symbols are already initialized to 0.
static size_t model_active_count;
static size_t model_memory;

// TODO: tidy up this function
model_mid_t model_load_obj(const char* asset, gfx_tid_t tid, bool textured)
{
    model_t model;
    model.faces      = NULL;
    model.face_count = 0;
    model.tid        = textured ? tid : GFX_UNUSED;
    model.textured   = textured;
    model.id        = 0;

    FILE* f = NULL;
    
    size_t vertex_count = 0;
    size_t uv_count     = 0;
    size_t face_count   = 0;
    
    float*        vertices = NULL;
    float*        uv       = NULL;
    

    char line[256] = {0}; // 255 characters for a line should be enough.

    if((f = fopen(asset, "r")) == NULL) {
        debug_printf(DEBUG_ERROR, "Couldn't open specified model: %s\n", asset);
        return MODEL_ERROR;
    }

    // Initially parse for amount of vertices, UVs and faces
    while(fgets(line, sizeof(line), f)) {
        if(line[0] == 'v' && isspace((int)line[1])) {
            vertex_count++;
        }
        else if(line[0] == 'v' && line[1] == 't') {
            uv_count++;
        }
        else if(line[0] == 'f') {
            face_count++;
        }
    }
    
    if((vertices = malloc(sizeof(float) * vertex_count * 3)) == NULL) {
        debug_printf(DEBUG_ERROR, "Failed to allocate memory for model vertices.\n");
        fclose(f);
        return MODEL_ERROR;
    }
    if((uv = malloc(sizeof(float) * uv_count * 2)) == NULL) {
        debug_printf(DEBUG_ERROR, "Failed to allocate memory for model UVs.\n");
        fclose(f);
        free(vertices);
        return MODEL_ERROR;
    }
    if((model.faces = malloc(sizeof(model_face_t) * face_count)) == NULL) {
        debug_printf(DEBUG_ERROR, "Failed to allocate memory for model faces.\n");
        fclose(f);
        free(vertices);
        free(uv);
        return MODEL_ERROR;
    }

    rewind(f);

    size_t vertex_idx = 0;
    size_t uv_idx     = 0;
    

    // Since vertices can be defined after faces, collect all vertex info first
    while(fgets(line, sizeof(line), f)) {
        if(line[0] == 'v' && isspace((int)line[1])) {
            float x, y, z;
            sscanf(line, "v %f %f %f", &x, &y, &z);
            vertices[vertex_idx++] = x;
            vertices[vertex_idx++] = y;
            vertices[vertex_idx++] = z;
        }
        else if(line[0] == 'v' && line[1] == 't') {
            float u, v;
            sscanf(line, "vt %f %f", &u, &v);
            uv[uv_idx++] = u;
            uv[uv_idx++] = v;
        }
    }

    rewind(f);

    size_t face_idx   = 0;

    while(fgets(line, sizeof(line), f)) {
        if(line[0] == 'f') {
            size_t va, vta, vna;
            size_t vb, vtb, vnb;
            size_t vc, vtc, vnc;
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",  &va, &vta, &vna,
                                                          &vb, &vtb, &vnb,
                                                          &vc, &vtc, &vnc);

            model.faces[face_idx].a.color.argb = 0xFFFFFFFF;
            model.faces[face_idx].a.position.x = vertices[(3*va)+0];
            model.faces[face_idx].a.position.y = vertices[(3*va)+1];
            model.faces[face_idx].a.position.z = vertices[(3*va)+2];
            model.faces[face_idx].a.u = uv[(2*vta)+0];
            model.faces[face_idx].a.v = uv[(2*vta)+1];

            model.faces[face_idx].b.color.argb = 0xFFFFFFFF;
            model.faces[face_idx].b.position.x = vertices[(3*vb)+0];
            model.faces[face_idx].b.position.y = vertices[(3*vb)+1];
            model.faces[face_idx].b.position.z = vertices[(3*vb)+2];
            model.faces[face_idx].b.u = uv[(2*vtb)+0];
            model.faces[face_idx].b.v = uv[(2*vtb)+1];

            model.faces[face_idx].c.color.argb = 0xFFFFFFFF;
            model.faces[face_idx].c.position.x = vertices[(3*vc)+0];
            model.faces[face_idx].c.position.y = vertices[(3*vc)+1];
            model.faces[face_idx].c.position.z = vertices[(3*vc)+2];
            model.faces[face_idx].c.u = uv[(2*vtc)+0];
            model.faces[face_idx].c.v = uv[(2*vtc)+1];

            face_idx++;
        }
    }

    free(vertices);
    free(uv);
    fclose(f);

    model_t* tmp = realloc(models, sizeof(model_t) * (model_id_count+1));
    if(tmp == NULL) {
        debug_printf(DEBUG_ERROR, "Failed to allocate memory for model array.\n");
        free(model.faces);
        return MODEL_ERROR;
    }

    models = tmp;

    model_id_count++;
    model_active_count++;
    model_memory += sizeof(model_face_t) * face_count;

    model.id = model_id_count-1;
    model.face_count = face_count;

    models[model.id] = model;

    debug_printf(DEBUG_INFO, "Loaded model (mid = %d)\n", model.id);
    debug_printf(DEBUG_BLANK,"Asset: %s\n", asset);
    debug_printf(DEBUG_BLANK,"Vertices: %d   Faces: %d\n", vertex_count, face_count);

    debug_printf(DEBUG_INFO, "Active models: %d\n", model_active_count);
    debug_printf(DEBUG_BLANK, "Model memory used: %.1f KiB\n", model_memory / 1024.0f);

    return model.id;
}

void model_free_obj(model_mid_t mid)
{
    model_active_count--;
    model_memory -= sizeof(model_face_t) * models[mid].face_count;
    models[mid].id = MODEL_FREED;

    debug_printf(DEBUG_INFO, "Freed model (mid = %d)\n", mid);
    debug_printf(DEBUG_INFO, "Active models: %d\n", model_active_count);
    debug_printf(DEBUG_BLANK,"Model memory used: %.1f KiB\n", model_memory/1024.0f);
}

void model_render_obj(model_mid_t mid)
{
    model_t model = models[mid];

    if(model.textured == true && model.tid != GFX_UNUSED) {
        for(size_t i = 0; i < model.face_count; i++) {
            gfx_vertex_t a = model.faces[i].a;
            gfx_vertex_t b = model.faces[i].b;
            gfx_vertex_t c = model.faces[i].c;
            a.position.x = ((CONFIG_RESOLUTION/2.5f)*a.position.x) + (CONFIG_SCREEN_W/2.0f);
            a.position.y = ((CONFIG_RESOLUTION/2.5f)*a.position.y) + (CONFIG_SCREEN_H/2.0f);
            a.position.z = a.position.z + 2.0f;

            b.position.x = ((CONFIG_RESOLUTION/2.5f)*b.position.x) + (CONFIG_SCREEN_W/2.0f);
            b.position.y = ((CONFIG_RESOLUTION/2.5f)*b.position.y) + (CONFIG_SCREEN_H/2.0f);
            b.position.z = b.position.z + 2.0f;
            
            c.position.x = ((CONFIG_RESOLUTION/2.5f)*c.position.x) + (CONFIG_SCREEN_W/2.0f);
            c.position.y = ((CONFIG_RESOLUTION/2.5f)*c.position.y) + (CONFIG_SCREEN_H/2.0f);
            c.position.z = c.position.z + 2.0f;

            gfx_draw_op_tex_tri(a, b, c, model.tid);
        }
    }
    else {
        for(size_t i = 0; i < model.face_count; i++) {
            gfx_draw_op_tri(model.faces[i].a, model.faces[i].b, model.faces[i].c);
        }
    }
}
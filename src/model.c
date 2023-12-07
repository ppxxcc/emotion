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


static model_t model[CONFIG_MAX_MODELS];
static bool    model_occupied[CONFIG_MAX_MODELS];
static size_t  model_count;
static size_t  model_memory;

void model_initialize(void)
{
    for(int i = 0; i < CONFIG_MAX_MODELS; i++) {
        model_t m = {0};
        model[i] = m;
        model_occupied[i] = false;
    }

    model_count  = 0;
    model_memory = 0;

    debug_printf(DEBUG_INFO, "Initialized model manager.\n");
    debug_printf(DEBUG_BLANK,"Model limit: %d\n", CONFIG_MAX_MODELS);
}


// TODO: tidy up this function
model_mid_t model_load_obj(const char* asset, gfx_tid_t tid, bool textured)
{
    if(model_count >= CONFIG_MAX_MODELS) {
        debug_printf(DEBUG_ERROR, "Cannot allocate more than %d models.\n", CONFIG_MAX_MODELS);
        return MODEL_ERROR;
    }

    model_t m;
    m.faces      = NULL;
    m.face_count = 0;
    m.tid        = textured ? tid : GFX_UNUSED;
    m.textured   = textured;
    m.mid        = 0;

    FILE* f = NULL;
    
    size_t vertex_count = 0;
    size_t uv_count     = 0;
    size_t face_count   = 0;
    
    float* vertices = NULL;
    float* uv       = NULL;
    
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
    if((m.faces = malloc(sizeof(model_face_t) * face_count)) == NULL) {
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

            m.faces[face_idx].a.color.argb = 0xFFFFFFFF;
            m.faces[face_idx].a.position.x = vertices[(3*va)+0];
            m.faces[face_idx].a.position.y = vertices[(3*va)+1];
            m.faces[face_idx].a.position.z = vertices[(3*va)+2];
            m.faces[face_idx].a.u = uv[(2*vta)+0];
            m.faces[face_idx].a.v = uv[(2*vta)+1];

            m.faces[face_idx].b.color.argb = 0xFFFFFFFF;
            m.faces[face_idx].b.position.x = vertices[(3*vb)+0];
            m.faces[face_idx].b.position.y = vertices[(3*vb)+1];
            m.faces[face_idx].b.position.z = vertices[(3*vb)+2];
            m.faces[face_idx].b.u = uv[(2*vtb)+0];
            m.faces[face_idx].b.v = uv[(2*vtb)+1];

            m.faces[face_idx].c.color.argb = 0xFFFFFFFF;
            m.faces[face_idx].c.position.x = vertices[(3*vc)+0];
            m.faces[face_idx].c.position.y = vertices[(3*vc)+1];
            m.faces[face_idx].c.position.z = vertices[(3*vc)+2];
            m.faces[face_idx].c.u = uv[(2*vtc)+0];
            m.faces[face_idx].c.v = uv[(2*vtc)+1];

            face_idx++;
        }
    }

    free(vertices);
    free(uv);
    fclose(f);

    model_mid_t first_available = 0;

    while(model_occupied[first_available] == true) {
        first_available++;
    }

    model_mid_t mid = first_available;

    model_occupied[mid] = true;

    model_count++;
    model_memory += sizeof(model_face_t) * face_count;

    m.mid = mid;
    m.face_count = face_count;

    model[mid] = m;

    debug_printf(DEBUG_INFO, "Loaded model (mid = %d)\n", mid);
    debug_printf(DEBUG_BLANK,"Asset: %s\n", asset);
    debug_printf(DEBUG_BLANK,"Vertices: %d   Faces: %d\n", vertex_count, face_count);

    debug_printf(DEBUG_INFO, "Active models: %d\n", model_count);
    debug_printf(DEBUG_BLANK, "Model memory used: %.1f KiB\n", model_memory / 1024.0f);

    return mid;
}

void model_free_obj(model_mid_t mid)
{
    if(mid >= CONFIG_MAX_MODELS || model_occupied[mid] == false) {
        debug_printf(DEBUG_ERROR, "Received invalid model ID (%d) for freeing.\n", mid);
        return;
    }

    free(model[mid].faces);

    model_count--;
    model_memory -= sizeof(model_face_t) * model[mid].face_count;
    model_occupied[mid] = false;

    debug_printf(DEBUG_INFO, "Freed model (mid = %d)\n", mid);
    debug_printf(DEBUG_INFO, "Active models: %d\n", model_count);
    debug_printf(DEBUG_BLANK,"Model memory used: %.1f KiB\n", model_memory/1024.0f);
}

void model_render_obj(model_mid_t mid)
{
    model_t m = model[mid];

    if(m.textured == true && m.tid != GFX_UNUSED) {
        for(size_t i = 0; i < m.face_count; i++) {
            gfx_vertex_t a = m.faces[i].a;
            gfx_vertex_t b = m.faces[i].b;
            gfx_vertex_t c = m.faces[i].c;
            a.position.x = ((CONFIG_RESOLUTION/2.5f)*a.position.x) + (CONFIG_SCREEN_W/2.0f);
            a.position.y = ((CONFIG_RESOLUTION/2.5f)*a.position.y) + (CONFIG_SCREEN_H/2.0f);
            a.position.z = a.position.z + 2.0f;

            b.position.x = ((CONFIG_RESOLUTION/2.5f)*b.position.x) + (CONFIG_SCREEN_W/2.0f);
            b.position.y = ((CONFIG_RESOLUTION/2.5f)*b.position.y) + (CONFIG_SCREEN_H/2.0f);
            b.position.z = b.position.z + 2.0f;
            
            c.position.x = ((CONFIG_RESOLUTION/2.5f)*c.position.x) + (CONFIG_SCREEN_W/2.0f);
            c.position.y = ((CONFIG_RESOLUTION/2.5f)*c.position.y) + (CONFIG_SCREEN_H/2.0f);
            c.position.z = c.position.z + 2.0f;

            gfx_draw_op_tex_tri(a, b, c, m.tid);
        }
    }
    else {
        for(size_t i = 0; i < m.face_count; i++) {
            gfx_draw_op_tri(m.faces[i].a, m.faces[i].b, m.faces[i].c);
        }
    }
}
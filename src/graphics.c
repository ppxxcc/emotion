// ============================================================================
// File:        graphics.c
// Description: Graphics related functionality (source)
// Author:      Shirobon
// Date:        2023/12/05
// ============================================================================

#include "config.h"

#include "graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <kos.h>

static gfx_texture_t* textures; // Array type of active textures

static size_t vertex_count;
static size_t vertex_memory;
static size_t texture_id_count;
static size_t texture_active_count;
static size_t texture_memory;

void gfx_initialize(void)
{
    textures              = NULL; // I know these should already be zero in bss
    vertex_count          = 0;
    vertex_memory         = 0;
    texture_id_count      = 0;
    texture_active_count  = 0;
    texture_memory        = 0;

    vid_set_mode(CONFIG_VIDEO_MODE, PM_RGB565);
    pvr_init_defaults();
}

void gfx_begin(void)
{
    vertex_count  = 0;
    vertex_memory = 0;

    pvr_wait_ready();
    pvr_scene_begin();
    pvr_list_begin(PVR_LIST_OP_POLY);
}

void gfx_end(void)
{
    pvr_list_finish();
    pvr_scene_finish();
}

gfx_tid_t gfx_load_texture(const char* asset, size_t width, size_t height)
{
    void* ptx;
    FILE* f;
    size_t size = width*height*2; // 16bpp/2 bytes per pixel

    if((f = fopen(asset, "rb")) == NULL) {
        printf("Couldn't open specified texture: %s\n", asset);
        return GFX_ERROR;
    }

    if((ptx = pvr_mem_malloc(size)) == NULL) {
        printf("Failed to allocate PVR memory for texture.\n");
        return GFX_ERROR;
    }

    if(fread(ptx, 1, size, f) != size) {
        printf("Error reading texture from: %s\n", asset);
        fclose(f);
        pvr_mem_free(ptx);
        return GFX_ERROR;
    }

    gfx_texture_t* tmp = realloc(textures, sizeof(gfx_texture_t) * (texture_id_count+1));
    if(tmp == NULL) {
        printf("Failed to allocate memory for texture array.\n");
        pvr_mem_free(ptx);
        return GFX_ERROR;
    }

    textures = tmp;

    texture_id_count++;
    texture_active_count++;
    texture_memory += size;

    gfx_tid_t id = id;

    textures[id].asset      = (char*)asset;
    textures[id].pvr_memory = ptx;
    textures[id].width      = width;
    textures[id].height     = height;
    textures[id].size       = size;
    textures[id].id         = id;

    return id;
}

void gfx_free_texture(gfx_tid_t tid)
{
    texture_active_count--;
    texture_memory -= textures[tid].size;
    pvr_mem_free(textures[tid].pvr_memory);
    textures[tid].id = GFX_FREED;
}

void gfx_draw_op_tri(gfx_vertex_t va, gfx_vertex_t vb, gfx_vertex_t vc)
{
    pvr_poly_hdr_t hdr;
    pvr_poly_cxt_t cxt;
    pvr_vertex_t   v;

    pvr_poly_cxt_col(&cxt, PVR_LIST_OP_POLY);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    v.flags = PVR_CMD_VERTEX;
    v.x = va.position.x;
    v.y = va.position.y;
    v.z = va.position.z;
    v.u = 0.0f;
    v.v = 0.0f;
    v.argb = va.color.argb;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    v.flags = PVR_CMD_VERTEX;
    v.x = vb.position.x;
    v.y = vb.position.y;
    v.z = vb.position.z;
    v.u = 0.0f;
    v.v = 0.0f;
    v.argb = vb.color.argb;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    v.flags = PVR_CMD_VERTEX_EOL;
    v.x = vc.position.x;
    v.y = vc.position.y;
    v.z = vc.position.z;
    v.u = 0.0f;
    v.v = 0.0f;
    v.argb = vc.color.argb;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    vertex_count  += 3;
    vertex_memory += sizeof(hdr) + 3*sizeof(v);
}

void gfx_draw_op_tex_tri(gfx_vertex_t va, gfx_vertex_t vb, gfx_vertex_t vc, gfx_tid_t tid)
{
    pvr_poly_hdr_t hdr;
    pvr_poly_cxt_t cxt;
    pvr_vertex_t   v;
    gfx_texture_t  txr;

    txr = textures[tid];

    pvr_poly_cxt_txr(&cxt,  PVR_LIST_OP_POLY,
                            PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED,
                            txr.width,
                            txr.height,
                            txr.pvr_memory,
                            PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    v.flags = PVR_CMD_VERTEX;
    v.x = va.position.x;
    v.y = va.position.y;
    v.z = va.position.z;
    v.u = va.u;
    v.v = va.v;
    v.argb = va.color.argb;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    v.flags = PVR_CMD_VERTEX;
    v.x = vb.position.x;
    v.y = vb.position.y;
    v.z = vb.position.z;
    v.u = vb.u;
    v.v = vb.v;
    v.argb = vb.color.argb;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    v.flags = PVR_CMD_VERTEX_EOL;
    v.x = vc.position.x;
    v.y = vc.position.y;
    v.z = vc.position.z;
    v.u = vc.u;
    v.v = vc.v;
    v.argb = vc.color.argb;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    vertex_count  += 3;
    vertex_memory += sizeof(hdr) + 3*sizeof(v);
}
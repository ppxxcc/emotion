// ============================================================================
// File:        graphics.c
// Description: Graphics related functionality (source)
// Author:      Shirobon
// Date:        2023/12/05
// ============================================================================

#include "config.h"

#include "graphics.h"

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <kos.h>

static gfx_texture_t texture[CONFIG_MAX_TEXTURES];
static bool          texture_occupied[CONFIG_MAX_TEXTURES];
static size_t        texture_count;
static size_t        texture_memory;
static size_t        vertex_count;
static size_t        vertex_memory;


void gfx_initialize(void)
{
    for(int i = 0; i < CONFIG_MAX_TEXTURES; i++) {
        gfx_texture_t t = {0};
        texture[i] = t;
        texture_occupied[i] = false;
    }
    
    texture_count   = 0;
    texture_memory  = 0;
    vertex_count    = 0;
    vertex_memory   = 0;

    debug_printf(DEBUG_INFO, "Initialized texture manager.\n");
    debug_printf(DEBUG_BLANK,"Texture limit: %d\n", CONFIG_MAX_TEXTURES);

    vid_set_mode(CONFIG_VIDEO_MODE, PM_RGB565);
    pvr_init_defaults();

    debug_printf(DEBUG_INFO, "Initialized video at %dx%d.\n", CONFIG_SCREEN_W, CONFIG_SCREEN_H);
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
    if(texture_count >= CONFIG_MAX_TEXTURES) {
        debug_printf(DEBUG_ERROR, "Cannot allocate more than %d textures.\n", CONFIG_MAX_TEXTURES);
        return GFX_ERROR;
    }

    void* ptx; // Pointer to texture memory for PVR
    FILE* f;
    size_t size = width*height*2; // 16bpp/2 bytes per pixel

    if((f = fopen(asset, "rb")) == NULL) {
        debug_printf(DEBUG_ERROR, "Couldn't open specified texture: %s\n", asset);
        return GFX_ERROR;
    }

    if((ptx = pvr_mem_malloc(size)) == NULL) {
        debug_printf(DEBUG_ERROR, "Failed to allocate PVR memory for texture.\n");
        return GFX_ERROR;
    }

    if(fread(ptx, 1, size, f) != size) {
        debug_printf(DEBUG_ERROR, "Failed to read texture from: %s\n", asset);
        fclose(f);
        pvr_mem_free(ptx);
        return GFX_ERROR;
    }

    fclose(f);

    gfx_tid_t first_available = 0;

    while(texture_occupied[first_available] == true) {
        first_available++;
    }

    gfx_tid_t tid = first_available;

    texture_occupied[tid] = true;

    texture_count++;
    texture_memory += size;

    texture[tid].asset      = (char*)asset;
    texture[tid].pvr_memory = ptx;
    texture[tid].width      = width;
    texture[tid].height     = height;
    texture[tid].size       = size;
    texture[tid].tid        = tid;

    debug_printf(DEBUG_INFO, "Loaded PVR texture (tid = %d)\n", tid);
    debug_printf(DEBUG_BLANK,"Asset: %s\n", asset);
    debug_printf(DEBUG_BLANK,"Size:  %dx%d\n", width, height);
    
    debug_printf(DEBUG_INFO, "Active textures: %d\n", texture_count);
    debug_printf(DEBUG_BLANK, "Texture memory used: %.1f KiB\n", texture_memory/1024.0f);

    return tid;
}

void gfx_free_texture(gfx_tid_t tid)
{
    if(tid >= CONFIG_MAX_TEXTURES || texture_occupied[tid] == false) {
        debug_printf(DEBUG_ERROR, "Received invalid texture id (%d) for freeing.\n", tid);
        return;
    }

    pvr_mem_free(texture[tid].pvr_memory);

    texture_count--;
    texture_memory -= texture[tid].size;
    texture_occupied[tid] = false;

    debug_printf(DEBUG_INFO, "Freed PVR texture (tid = %d)\n", tid);
    debug_printf(DEBUG_INFO, "Active textures: %d\n", texture_count);
    debug_printf(DEBUG_BLANK,"Texture memory used: %.1f KiB\n", texture_memory/1024.0f);
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

    txr = texture[tid];

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

void gfx_draw_op_tex1555_tri(gfx_vertex_t va, gfx_vertex_t vb, gfx_vertex_t vc, gfx_tid_t tid)
{
    pvr_poly_hdr_t hdr;
    pvr_poly_cxt_t cxt;
    pvr_vertex_t   v;
    gfx_texture_t  txr;

    txr = texture[tid];

    pvr_poly_cxt_txr(&cxt,  PVR_LIST_OP_POLY,
                            PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED,
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

void gfx_font_printf(gfx_tid_t tid, float size, float x, float y, const char* fmt, ...)
{
    char buffer[256];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, 256, fmt, args);
    va_end(args);

    size_t length = strlen(buffer);

    for(size_t i = 0; i < length; i++) {
        char c = buffer[i];
        unsigned int offset_y = (unsigned int)c / 16;
        unsigned int offset_x = (unsigned int)c % 16;
        float uv_increment = 1.0f / 16.0f;

        float u_topleft  = uv_increment * offset_x;
        float v_topleft  = uv_increment * offset_y;
        
        float u_topright = u_topleft + uv_increment;
        float v_topright = v_topleft;

        float u_bottomright = u_topleft + uv_increment;
        float v_bottomright = v_topleft + uv_increment;

        float u_bottomleft = u_topleft;
        float v_bottomleft = v_topleft + uv_increment;

        gfx_vertex_t va = {{x,      y,      10000.0f}, u_topleft,     v_topleft,     {0xFFFF0000}};
        gfx_vertex_t vb = {{x+size, y+size, 10000.0f}, u_bottomright, v_bottomright, {0xFF00FF00}};
        gfx_vertex_t vc = {{x,      y+size, 10000.0f}, u_bottomleft,  v_bottomleft,  {0xFF0000FF}};
        gfx_vertex_t vd = {{x+size, y,      10000.0f}, u_topright,    v_topright,    {0xFFFFFF00}};

        gfx_draw_op_tex1555_tri(va, vb, vc, tid);
        gfx_draw_op_tex1555_tri(va, vd, vb, tid);

        x += size; // Increment position 16 pixels to the right for next character
    }
}

gfx_vram_info_t gfx_get_vram_info(void)
{
    return (gfx_vram_info_t) { texture_count, texture_memory, vertex_count, vertex_memory };
}
// ============================================================================
// File:        graphics.h
// Description: Graphics related functionality (header)
// Author:      Shirobon
// Date:        2023/12/05
// ============================================================================

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>
#include <kos.h>

typedef uint8_t gfx_tid_t;

#define GFX_ERROR  (255)
#define GFX_FREED  (254)
#define GFX_UNUSED (253)

typedef union gfx_color_t
{
    uint32_t argb;
    struct {
        uint8_t b, g, r, a; // Little endian machine
    } component;
} gfx_color_t;

typedef struct gfx_vertex_t
{
    struct {
        float x, y, z;
    } position;        // 3D position in space
    float u, v;        // Texture coordinates
    gfx_color_t color; // 32-bit ARGB
} gfx_vertex_t;

typedef struct gfx_texture_t
{
    char* asset;
    void* pvr_memory;
    size_t width, height, size;
    gfx_tid_t tid;
} gfx_texture_t;

void gfx_initialize(void);
void gfx_begin(void);
void gfx_end(void);

gfx_tid_t gfx_load_texture(const char* asset, size_t width, size_t height);
void      gfx_free_texture(gfx_tid_t tid);

void    gfx_draw_op_tri(gfx_vertex_t va, gfx_vertex_t vb, gfx_vertex_t vc);
void    gfx_draw_op_tex_tri(gfx_vertex_t va, gfx_vertex_t vb, gfx_vertex_t vc, gfx_tid_t tid);



#endif // GRAPHICS_H
// ============================================================================
// File:        main.c
// Description: "Emotion" demo entry point
// Author:      Shirobon
// Date:        2023/12/04
// ============================================================================

#include <kos.h>

#include "config.h"
#include "debug.h"
#include "controller.h"
#include "graphics.h"

int main(void)
{
    debug_begin(DEBUG_STDOUT | DEBUG_FILE);

    gfx_initialize();

    controller_initialize();

    gfx_tid_t mytexture = gfx_load_texture("/rd/asset/texture/earth_1024x512.565", 1024, 512);
    
    if(mytexture == GFX_ERROR) {
        debug_printf(DEBUG_ERROR, "Couldn't load texture! Exiting.\n");
        return 0;
    }

    gfx_vertex_t a = {
                .position = { 80.0f, 40.0f, 1.0f },
                .u = 0.0f, .v = 0.0f,
                .color = { 0xFFFFFFFF }
                };
    gfx_vertex_t b = {
                .position = { 240.0f, 40.0f, 1.0f },
                .u = 1.0f, .v = 0.0f,
                .color = { 0xFFFFFFFF }
                };
    gfx_vertex_t c = {
                .position = { 240.0f, 200.0f, 1.0f },
                .u = 1.0f, .v = 1.0f,
                .color = { 0xFFFFFFFF }
                };
    gfx_vertex_t d = {
                .position = { 80.0f, 200.0f, 1.0f },
                .u = 0.0f, .v = 1.0f,
                .color = { 0xFFFFFFFF }
                };

    while(1) {
        controller_read_state();
        if(controller_test_button(CONTROLLER_START, CONTROLLER_PRESSED)) {
            debug_printf(DEBUG_INFO, "Start was pressed. Exiting demo.\n");
            break;
        }

        gfx_begin();

        gfx_draw_op_tex_tri(a, c, d, mytexture);
        gfx_draw_op_tex_tri(a, b, c, mytexture);

        gfx_end();


    }

    debug_end();

    return 0;
}

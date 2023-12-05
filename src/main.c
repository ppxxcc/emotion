// ============================================================================
// File:        main.c
// Description: "Emotion" demo entry point
// Author:      Shirobon
// Date:        2023/12/04
// ============================================================================

#include <kos.h>

#include "config.h"
#include "controller.h"
#include "graphics.h"

int main(void)
{
    printf("'Emotion' by Shirobon\n");

    gfx_initialize();

    controller_initialize();

    gfx_tid_t mytexture = gfx_load_texture("/rd/asset/texture/drmcsat_128x128.565", 128, 128);
    if(mytexture == GFX_ERROR) {
        printf("Couldn't allocate texture! Exiting.\n");
        return 0;
    }

    gfx_vertex_t a = {
                .position = { 80.0f, 40.0f, 1.0f },
                .u = 0.0f, .v = 0.0f,
                .color = { 0xFFFF0000 }
                };
    gfx_vertex_t b = {
                .position = { 240.0f, 40.0f, 1.0f },
                .u = 1.0f, .v = 0.0f,
                .color = { 0xFF00FF00 }
                };
    gfx_vertex_t c = {
                .position = { 240.0f, 200.0f, 1.0f },
                .u = 1.0f, .v = 1.0f,
                .color = { 0xFF0000FF }
                };
    gfx_vertex_t d = {
                .position = { 80.0f, 200.0f, 1.0f },
                .u = 0.0f, .v = 1.0f,
                .color = { 0xFFFFFF00 }
                };

    while(1) {
        controller_read_state();
        if(controller_test_button(CONTROLLER_START, CONTROLLER_PRESSED)) {
            printf("Exiting demo.\n");
            break;
        }

        gfx_begin();

        gfx_draw_op_tex_tri(a, c, d, mytexture);
        gfx_draw_op_tex_tri(a, b, c, mytexture);

        gfx_end();


    }

    return 0;
}

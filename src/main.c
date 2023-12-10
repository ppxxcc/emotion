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
#include "model.h"
#include "mv.h"

int main(void)
{
    debug_begin(CONFIG_DEBUG_DEFAULT_MODE);

    gfx_initialize();

    model_initialize();

    controller_initialize();

    mv_set_matrix_model(MV_PROJECTION);
    mv_identity();

    mv_set_matrix_model(MV_MODELVIEW);
    mv_identity();

    gfx_tid_t font_texture = gfx_load_texture("/rd/asset/texture/font_256x256.1555", 256, 256);

    gfx_tid_t bruce_texture = gfx_load_texture("/rd/asset/texture/bruce_128x128.565", 128, 128);

    gfx_tid_t earth_texture = gfx_load_texture("/rd/asset/texture/earth_512x512.565", 512, 512);

    model_mid_t cube_model = model_load_obj("/rd/asset/model/cube.obj", bruce_texture, true);
    model_mid_t sphere_model = model_load_obj("/rd/asset/model/sphere_lowpoly.obj", earth_texture, true);

    while(1) {
        static float angle = MV_PIDIV2;
        static gfx_vram_info_t vram = {0};
        angle += 0.01f;
        if(angle >= 6.28f) {
           angle = 0.0f;
        }

        controller_read_state();
        if(controller_test_button(CONTROLLER_START, CONTROLLER_PRESSED)) {
            debug_printf(DEBUG_INFO, "Start was pressed. Exiting demo.\n");
            break;
        }

        mv_identity(); // start with identity matrix

        float scalew = (CONFIG_SCREEN_W/10.0f/1.33f);
        float scaleh = (CONFIG_SCREEN_W/15.0f);
        float posx   = (CONFIG_SCREEN_W/2.0f);
        float posy   = (CONFIG_SCREEN_H/1.9f);
        float posz   = 500.0f;
        float factor = 3.0f;
        // Cube transform
        float cube_scale = (scaleh-(scaleh*fsin(factor*angle)*1.2)/4.0f)+10.0f;
        mv_scale(cube_scale, cube_scale, 1.0f);
        mv_translate(posx+(posx*fcos(factor*angle)*0.3), posy, posz-(posz*fsin(factor*angle)*0.8));
        mv_rotate(angle*4.0f, 1.0, 1.0f, 1.0f);
        mv_rotate(angle*6.0f, 1.0f, 0.0f, 0.0f);
        mv_rotate(angle, 1.0f, 0.0f, 1.0f);
        mv_push_matrix(); // save cube

        mv_identity(); // start with identity

        // sphere transform
        float sphere_scale = scaleh+(scaleh*fsin(factor*angle)*1.2)+30.0f;
        mv_scale(sphere_scale, sphere_scale, 1.0f);
        mv_translate(posx-(posx*fcos(factor*angle)*0.3), posy, posz+(posz*fsin(factor*angle)*0.8));
        mv_rotate(angle*3.0f, 0.0, 1.0f, 0.0f);
        mv_rotate(MV_DEG_TO_RAD(25), 1.0f, 0.0f, 0.0f);

        gfx_begin();
        {
            mv_calculate_transform();
            model_render_obj(sphere_model);

            mv_pop_matrix();
            mv_calculate_transform();
            model_render_obj(cube_model);
           




            gfx_font_printf(font_texture, 64+(20*fsin(6.0f*angle)), 140-(40*fsin(6.0f*angle)), 40, "BRUCK");

            
            gfx_font_printf(font_texture, 16, 30, 380, "Vertices: %4d", vram.vertex_count);
            gfx_font_printf(font_texture, 16, 30, 400, "Textures: %4d", vram.texture_count);
            gfx_font_printf(font_texture, 16, 30, 420, "VRAM:      %6.2f KiB", (vram.vertex_memory + vram.texture_memory) / 1024.0f);
            gfx_font_printf(font_texture, 16, 30, 440, "Angle:       %4.2f rad", angle);
        }
        gfx_end();

        vram = gfx_get_vram_info();

    }

    gfx_free_texture(earth_texture);
    gfx_free_texture(bruce_texture);
    gfx_free_texture(font_texture);
    model_free_obj(sphere_model);
    model_free_obj(cube_model);

    debug_end();
    return 0;
}

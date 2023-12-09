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

    gfx_tid_t   earth_texture = gfx_load_texture("/rd/asset/texture/earth_512x512.565", 512, 512);
    gfx_tid_t   cube_texture  = gfx_load_texture("/rd/asset/texture/drmcsat_128x128.565", 128, 128);
    
    model_mid_t earth_model   = model_load_obj("/rd/asset/model/earth.obj", earth_texture, true);
    model_mid_t cube_model    = model_load_obj("/rd/asset/model/drmcsat_cube.obj", cube_texture, true);

    while(1) {
        static float angle = 0.0f;
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

        // torus transform
        mv_scale(CONFIG_SCREEN_W/6.0f/1.33f, CONFIG_SCREEN_H/6.0f, 1.0f);
        mv_translate((CONFIG_SCREEN_W/4.0f)*3.0f, CONFIG_SCREEN_H/2.0f, 500.0f);
        mv_rotate(angle, 0.0f, 1.0f, 0.0f);
        mv_rotate(angle, 1.0f, 1.0f, 1.0f);

        mv_push_matrix(); // save torus transform

        mv_identity(); // start with identity

        // earth transform
        mv_scale(CONFIG_SCREEN_W/3.3f/1.33f, CONFIG_SCREEN_H/3.3f, 1.0f);
        mv_translate(CONFIG_SCREEN_W/4.0f, CONFIG_SCREEN_H/2.0f, 500.0f);
        mv_rotate(angle*2.0f, 0.0f, 1.0f, 0.0f);
        //mv_rotate(angle, 1.0f, 1.0f, 0.0f);

        gfx_begin();

        mv_calculate_transform();
        model_render_obj(earth_model);

        mv_pop_matrix(); // get torus transform

        mv_calculate_transform();
        model_render_obj(cube_model);

        gfx_end();

    }

    gfx_free_texture(earth_texture);
    gfx_free_texture(cube_texture);
    model_free_obj(earth_model);
    model_free_obj(cube_model);

    debug_end();
    return 0;
}

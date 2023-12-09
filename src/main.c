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

    mv_scale(CONFIG_SCREEN_W/2.2f/1.33f, CONFIG_SCREEN_H/2.2f, 1.0f);

    mv_translate(CONFIG_SCREEN_W/2.0f, CONFIG_SCREEN_H/2.0f, 2.0f);

    gfx_tid_t   earth_texture = gfx_load_texture("/rd/asset/texture/earth_512x512.565", 512, 512);
    
    if(earth_texture == GFX_ERROR) {
        debug_printf(DEBUG_ERROR, "Couldn't load texture! Exiting.\n");
        return 0;
    }

    model_mid_t earth_model   = model_load_obj("/rd/asset/model/earth.obj", earth_texture, true);

    if(earth_model == MODEL_ERROR) {
        debug_printf(DEBUG_ERROR, "Couldn't load model! Exiting.\n");
        return 0;
    }

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

        mv_push_matrix();
        mv_rotate(angle, 0.0f, 1.0f, 0.0f);
        mv_rotate(angle, 1.0f, 1.0f, 1.0f);
        mv_calculate_transform();

        gfx_begin();

        model_render_obj(earth_model);

        gfx_end();

        mv_pop_matrix();

    }

    gfx_free_texture(earth_texture);
    model_free_obj(earth_model);

    debug_end();
    return 0;
}

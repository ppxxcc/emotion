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
#include "light.h"

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
    gfx_tid_t earth_texture = gfx_load_texture("/rd/asset/texture/earth_512x512.565", 512, 512);

    model_mid_t uvsphere_model  = model_load_obj("/rd/asset/model/uvsphere_medium.obj", GFX_UNUSED, false);
    model_mid_t icosphere_model = model_load_obj("/rd/asset/model/icosphere_medium.obj", GFX_UNUSED, false);

    gfx_vram_info_t vram = {0};



    while(1) {

        float scale  = 150.0f;
        float posx   = 320.0f;
        float posy   = 240.0f;
        float posz   = 500.0f;

        static gfx_color_t ambient_color     = {0xFFFFFFFF};
        static float       ambient_intensity = 0.1f;

        static gfx_color_t point_color       = {0xFFFFFFFF};
        
        static float       point_intensity   = 0.1f;
        static vec3_t      point_position    = {320.0f, 200.0f, 800.0f};


        controller_read_state();
        if(controller_test_button(CONTROLLER_START, CONTROLLER_PRESSED)) {
            debug_printf(DEBUG_INFO, "Start was pressed. Exiting demo.\n");
            break;
        }

        if(controller_test_button(CONTROLLER_A, CONTROLLER_PRESSED)) {
            ambient_intensity -= 0.01f;
            if(ambient_intensity < 0.0f) ambient_intensity = 0.0f;
        }

        if(controller_test_button(CONTROLLER_B, CONTROLLER_PRESSED)) {
            ambient_intensity += 0.01f;
            if(ambient_intensity > 1.0f) ambient_intensity = 1.0f;
        }

        if(controller_test_button(CONTROLLER_X, CONTROLLER_PRESSED)) {
            point_intensity -= 0.01f;
            if(point_intensity < 0.0f) point_intensity = 0.0f;
        }

        if(controller_test_button(CONTROLLER_Y, CONTROLLER_PRESSED)) {
            point_intensity += 0.01f;
            if(point_intensity > 1.0f) point_intensity = 1.0f;
        }

        if(controller_test_button(CONTROLLER_DPAD_LEFT, CONTROLLER_PRESSED)) {
            point_position.x -= 8.0f;
            if(point_position.x < 0.0f) point_position.x = 0.0f;
        }

        if(controller_test_button(CONTROLLER_DPAD_RIGHT, CONTROLLER_PRESSED)) {
            point_position.x += 8.0f;
            if(point_position.x > 640.0f) point_position.x = 640.0f;
        }

        if(controller_test_button(CONTROLLER_DPAD_UP, CONTROLLER_PRESSED)) {
            point_position.y -= 8.0f;
            if(point_position.y < 0.0f) point_position.y = 0.0f;
        }

        if(controller_test_button(CONTROLLER_DPAD_DOWN, CONTROLLER_PRESSED)) {
            point_position.y += 8.0f;
            if(point_position.y > 480.0f) point_position.y = 480.0f;
        }

        mv_identity(); // start with identity matrix


        // Icosphere Transform
        mv_scale(scale, scale, scale);
        mv_translate(posx, posy, posz);
        mv_calculate_transform();

        light_set_ambient((light_t){LIGHT_AMBIENT, {0.0f, 0.0f, 0.0f}, ambient_color, ambient_intensity});
        light_set_point(  (light_t){LIGHT_POINT,   point_position,     point_color,   point_intensity});

        gfx_begin();
        {
            
            model_render_obj(icosphere_model);

            gfx_font_printf(font_texture, 16, 20, 20, "Ambient RGB: <%03d,%03d,%03d> @ %.1f%%", ambient_color.component.r, ambient_color.component.g, ambient_color.component.b, ambient_intensity * 100.0f);
            gfx_font_printf(font_texture, 16, 20, 40, "Point   RGB: <%03d,%03d,%03d> @ %.1f%%", point_color.component.r, point_color.component.g, point_color.component.b, point_intensity * 100.0f);
            gfx_font_printf(font_texture, 16, 20, 60, "Point   XYZ: <%03d,%03d,%03d>", (int)point_position.x, (int)point_position.y, (int)point_position.z);
            gfx_font_printf(font_texture, 16, 20, 420, "Vertices: %4d", vram.vertex_count);
            gfx_font_printf(font_texture, 16, 20, 440, "Textures: %4d", vram.texture_count);
            gfx_font_printf(font_texture, 16, 350, 440, "VRAM: %6.2f KiB", (vram.vertex_memory + vram.texture_memory) / 1024.0f);
        }
        gfx_end();

        vram = gfx_get_vram_info();

    }


    model_free_obj(icosphere_model);
    model_free_obj(uvsphere_model);
    gfx_free_texture(font_texture);
    gfx_free_texture(earth_texture);

    debug_end();
    return 0;
}

// ============================================================================
// File:        light.c
// Description: Light functionality (implementation)
// Author:      Shirobon
// Date:        2023/12/16
// ============================================================================

#include "config.h"
#include "debug.h"
#include "light.h"
#include "mv.h"
#include "graphics.h"

static light_t ambient;
static light_t point;

static gfx_color_t color;

void light_set_ambient(light_t l)
{
    ambient = l;
}

void light_set_point(light_t l)
{
    point = l;
}

gfx_color_t light_calculate_color(vec3_t vertex_position, vec3_t normal)
{
    float       ambient_intensity = ambient.intensity;
    gfx_color_t ambient_color     = ambient.color;

    float ambient_r = ambient_intensity * ((float)ambient_color.component.r / 255.0f);
    float ambient_g = ambient_intensity * ((float)ambient_color.component.g / 255.0f);
    float ambient_b = ambient_intensity * ((float)ambient_color.component.b / 255.0f);

    float       point_intensity = point.intensity;
    gfx_color_t point_color     = point.color;

    vec3_t vertex_to_light = mv_vec_sub(point.position, vertex_position);
    float directness = mv_vec_scalar_product(mv_vec_normalize(vertex_to_light), mv_vec_normalize(normal));
    
    if(directness < 0.0f) {
        directness = 0.0f;
    }

    //directness = 0.0f;

    float point_r = directness * point_intensity * ((float)point_color.component.r / 255.0f);
    float point_g = directness * point_intensity * ((float)point_color.component.g / 255.0f);
    float point_b = directness * point_intensity * ((float)point_color.component.b / 255.0f);

    float total_r = ambient_r + point_r;
    float total_g = ambient_g + point_g;
    float total_b = ambient_b + point_b;

    if(total_r > 1.0f) total_r = 1.0f;
    if(total_g > 1.0f) total_g = 1.0f;
    if(total_b > 1.0f) total_b = 1.0f;

    color.component.a = 255;
    color.component.r = (uint8_t)(total_r * 255);
    color.component.g = (uint8_t)(total_g * 255);
    color.component.b = (uint8_t)(total_b * 255);

    return color;
}


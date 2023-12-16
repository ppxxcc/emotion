// ============================================================================
// File:        light.h
// Description: Light functionality (header)
// Author:      Shirobon
// Date:        2023/12/16
// ============================================================================

#ifndef LIGHT_H
#define LIGHT_H

#include "mv.h"
#include "graphics.h"

typedef enum light_type_t
{
    LIGHT_AMBIENT, LIGHT_POINT
} light_type_t;

typedef struct light_t
{
    light_type_t type;
    vec3_t       position;
    gfx_color_t  color;
    float        intensity;
} light_t;

void light_set_ambient(light_t l);
void light_set_point(light_t l);

gfx_color_t light_calculate_color(vec3_t vertex_position, vec3_t normal);


#endif // LIGHT_H

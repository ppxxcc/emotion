// ============================================================================
// File:        controller.c
// Description: Controller related functionality (source)
// Author:      Shirobon
// Date:        2023/12/04
// ============================================================================

#include "controller.h"

static maple_device_t*    device;
static controller_state_t state;

bool controller_initialize(void)
{
    device = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    
    if(device == NULL) {
        return false;
    }
    
    return true;
}

bool controller_read_state(void)
{
    cont_state_t* raw_state;
    
    raw_state = (cont_state_t*)maple_dev_status(device);

    if(raw_state == NULL) {
        return false;
    }

    state.buttons   = raw_state->buttons;
    state.trigger_l = raw_state->ltrig;
    state.trigger_r = raw_state->rtrig;
    state.stick_x   = raw_state->joyx;
    state.stick_y   = raw_state->joyy;

    return true;
}

void controller_print_state(void)
{
    #define b(x) (state.buttons & CONTROLLER_##x ? 1 : 0)
    #define d(x) (state.buttons & CONTROLLER_DPAD_##x ? 1 : 0)
    printf("Controller State:\n");
    printf(" A:%d  B:%d  X:%d  Y:%d S:%d\n", b(A), b(B), b(X), b(Y), b(START));
    printf(" U:%d  D:%d  L:%d  R:%d\n", d(UP), d(DOWN), d(LEFT), d(RIGHT));
    printf(" L:%04d  R:%04d\n", state.trigger_l, state.trigger_r);
    printf(" X:%04d  Y:%04d\n", state.stick_x, state.stick_y);
    #undef b
    #undef d
}

controller_state_t controller_get_state(void)
{
    return state;
}

bool controller_test_button(uint32_t button, bool expected)
{
    return (bool)(state.buttons & button) == expected;
}
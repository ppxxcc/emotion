// ============================================================================
// File:        controller.h
// Description: Controller related functionality (header)
// Author:      Shirobon
// Date:        2023/12/04
// ============================================================================

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>
#include <kos.h>

#define CONTROLLER_B          (1 << 1)
#define CONTROLLER_A          (1 << 2)
#define CONTROLLER_START      (1 << 3)
#define CONTROLLER_DPAD_UP    (1 << 4)
#define CONTROLLER_DPAD_DOWN  (1 << 5)
#define CONTROLLER_DPAD_LEFT  (1 << 6)
#define CONTROLLER_DPAD_RIGHT (1 << 7)
#define CONTROLLER_Y          (1 << 9)
#define CONTROLLER_X          (1 << 10)

#define CONTROLLER_PRESSED    (true)
#define CONTROLLER_UNPRESSED  (false)

typedef struct controller_state_t
{
    uint32_t buttons;
    uint8_t  trigger_l; // 0 (unpressed) to 255 (fully pressed)
    uint8_t  trigger_r; // 0 (unpressed) to 255 (fully pressed)
    int8_t   stick_x;   // -128 (left) to 127 (right)
    int8_t   stick_y;   // -128 (up) to 127 (down)
} controller_state_t;

bool controller_initialize(void);
bool controller_read_state(void);
void controller_print_state(void);

controller_state_t controller_get_state(void);

bool controller_test_button(uint32_t button, bool expected);

#endif // CONTROLLER_H
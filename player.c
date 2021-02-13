#include <stdio.h>
#include <stdbool.h>
#include "ui.h"

struct player {
    int position_x;
    int position_y;
    int lives;
    int fired;
    int missile_x;
    int missile_y;
} player = {WIDTH / 2, HEIGHT - 2, 3, false, 0, 0};

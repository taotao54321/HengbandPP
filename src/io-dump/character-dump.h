#pragma once

#include "io/files-util.h"
#include "system/angband.h"

void make_character_dump(player_type* creature_ptr, FILE* fff, void (*update_playtime)(void), display_player_pf display_player);

#pragma once

#include "io/files-util.h"
#include "system/angband.h"

void print_tomb(player_type* dead_ptr);
void show_death_info(player_type* creature_ptr, update_playtime_pf update_playtime, display_player_pf display_player);

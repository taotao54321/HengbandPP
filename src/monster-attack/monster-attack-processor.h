﻿#pragma once

#include "grid/grid.h"
#include "monster/monster-processor-util.h"
#include "system/angband.h"

void exe_monster_attack_to_player(player_type* target_ptr, turn_flags* turn_flags_ptr, MONSTER_IDX m_idx, POSITION ny, POSITION nx);
bool process_monster_attack_to_monster(player_type* target_ptr, turn_flags* turn_flags_ptr, MONSTER_IDX m_idx, grid_type* g_ptr, bool can_cross);

﻿#pragma once

#include "system/angband.h"

bool get_enemy_dir(player_type* target_ptr, MONSTER_IDX m_idx, int* mm);
bool decide_monster_movement_direction(player_type* target_ptr, DIRECTION* mm, MONSTER_IDX m_idx, bool aware);

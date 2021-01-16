#pragma once

#include "monster/monster-processor-util.h"
#include "system/angband.h"

bool find_safety(player_type* target_ptr, MONSTER_IDX m_idx, POSITION* yp, POSITION* xp);
bool find_hiding(player_type* target_ptr, MONSTER_IDX m_idx, POSITION* yp, POSITION* xp);

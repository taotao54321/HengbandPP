﻿#pragma once

#include "system/angband.h"
#include "system/monster-type-definition.h"

MONSTER_IDX m_pop(floor_type* floor_ptr);

#define GMN_ARENA 0x00000001 //!< 賭け闘技場向け生成
MONRACE_IDX get_mon_num(player_type* player_ptr, DEPTH level, BIT_FLAGS option);
void choose_new_monster(player_type* player_ptr, MONSTER_IDX m_idx, bool born, MONRACE_IDX r_idx);
SPEED get_mspeed(floor_type* player_ptr, monster_race* r_ptr);
int get_monster_crowd_number(floor_type* floor_ptr, MONSTER_IDX m_idx);

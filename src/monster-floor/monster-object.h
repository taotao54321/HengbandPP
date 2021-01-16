﻿#pragma once

#include "monster/monster-processor-util.h"
#include "system/angband.h"

void update_object_by_monster_movement(player_type* target_ptr, turn_flags* turn_flags_ptr, MONSTER_IDX m_idx, POSITION ny, POSITION nx);
void monster_drop_carried_objects(player_type* player_ptr, monster_type* m_ptr);

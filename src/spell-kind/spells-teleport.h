﻿#pragma once

#include "spell/spells-util.h"
#include "system/angband.h"

bool teleport_monster(player_type *caster_ptr, DIRECTION dir, int distance);
bool teleport_swap(player_type *caster_ptr, DIRECTION dir);
bool teleport_away(player_type *caster_ptr, MONSTER_IDX m_idx, POSITION dis, teleport_flags mode);
void teleport_monster_to(player_type *caster_ptr, MONSTER_IDX m_idx, POSITION ty, POSITION tx, int power, teleport_flags mode);
bool teleport_player_aux(player_type *creature_ptr, POSITION dis, bool is_quantum_effect, teleport_flags mode);
void teleport_player(player_type *creature_ptr, POSITION dis, teleport_flags mode);
void teleport_player_away(MONSTER_IDX m_idx, player_type *target_ptr, POSITION dis, bool is_quantum_effect);
void teleport_player_to(player_type *creature_ptr, POSITION ny, POSITION nx, teleport_flags mode);
void teleport_away_followable(player_type *creature_ptr, MONSTER_IDX m_idx);
bool dimension_door(player_type *caster_ptr);
bool exe_dimension_door(player_type *caster_ptr, POSITION x, POSITION y);

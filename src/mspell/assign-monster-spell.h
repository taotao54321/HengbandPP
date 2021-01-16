﻿#pragma once

#include "system/angband.h"

HIT_POINT monspell_to_player(player_type* target_ptr, SPELL_IDX ms_type, POSITION y, POSITION x, MONSTER_IDX m_idx);
HIT_POINT monspell_to_monster(player_type* target_ptr, SPELL_IDX ms_type, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, bool is_special_spell);

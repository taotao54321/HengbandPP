﻿#pragma once

#include "system/angband.h"

#include "spell/summon-types.h"

typedef bool (*monsterrace_hook_type)(player_type *, MONRACE_IDX);

extern MONSTER_IDX hack_m_idx;
extern MONSTER_IDX hack_m_idx_ii;
extern int chameleon_change_m_idx;
extern summon_type summon_specific_type;

monsterrace_hook_type get_monster_hook(player_type *player_ptr);
monsterrace_hook_type get_monster_hook2(player_type *player_ptr, POSITION y, POSITION x);
errr get_mon_num_prep(player_type *player_ptr, monsterrace_hook_type monster_hook, monsterrace_hook_type monster_hook2);

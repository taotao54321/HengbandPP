﻿#pragma once

#include "effect/effect-monster-util.h"
#include "system/angband.h"

bool affect_monster(player_type* caster_ptr, MONSTER_IDX who, POSITION r, POSITION y, POSITION x, HIT_POINT dam, EFFECT_ID typ, BIT_FLAGS flag, bool see_s_msg);

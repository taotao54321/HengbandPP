#pragma once

#include "floor/floor-allocation-types.h"
#include "system/angband.h"

bool alloc_stairs(player_type* owner_ptr, FEAT_IDX feat, int num, int walls);
void alloc_object(player_type* owner_ptr, dap_type set, EFFECT_ID typ, int num);

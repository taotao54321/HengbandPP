﻿#pragma once

#include "object/tval-types.h"
#include "system/angband.h"

COMMAND_CODE show_inventory(player_type* owner_ptr, int target_item, BIT_FLAGS mode, tval_type tval);
COMMAND_CODE show_equipment(player_type* owner_ptr, int target_item, BIT_FLAGS mode, tval_type tval);

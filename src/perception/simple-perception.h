﻿#pragma once

#include "object-enchant/item-feeling.h"
#include "system/angband.h"

void sense_inventory1(player_type* creature_ptr);
void sense_inventory2(player_type* creature_ptr);
item_feel_type pseudo_value_check_heavy(object_type* o_ptr);
item_feel_type pseudo_value_check_light(object_type* o_ptr);

﻿#pragma once

#include "system/angband.h"

void vary_item(player_type* owner_ptr, INVENTORY_IDX item, ITEM_NUMBER num);
void inven_item_increase(player_type* owner_ptr, INVENTORY_IDX item, ITEM_NUMBER num);
void inven_item_optimize(player_type* owner_ptr, INVENTORY_IDX item);
void drop_from_inventory(player_type* owner_type, INVENTORY_IDX item, ITEM_NUMBER amt);
void combine_pack(player_type* owner_ptr);
void reorder_pack(player_type* owner_ptr);
s16b store_item_to_inventory(player_type* owner_ptr, object_type* o_ptr);
bool check_store_item_to_inventory(player_type* owner_ptr, object_type* o_ptr);
INVENTORY_IDX inven_takeoff(player_type* owner_ptr, INVENTORY_IDX item, ITEM_NUMBER amt);

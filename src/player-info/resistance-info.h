#pragma once

#include "system/angband.h"

typedef struct self_info_type self_info_type;
void set_element_resistance_info(player_type* creature_ptr, self_info_type* self_ptr);
void set_high_resistance_info(player_type* creature_ptr, self_info_type* self_ptr);

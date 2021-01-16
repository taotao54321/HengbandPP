﻿#pragma once

#include "system/angband.h"

void player_immunity(player_type* creature_ptr, BIT_FLAGS* flags);
void tim_player_immunity(player_type* creature_ptr, BIT_FLAGS* flags);
void known_obj_immunity(player_type* creature_ptr, BIT_FLAGS* flags);
void player_vulnerability_flags(player_type* creature_ptr, BIT_FLAGS* flags);

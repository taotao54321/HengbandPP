﻿#pragma once

#include "player-attack/player-attack-util.h"
#include "system/angband.h"

void change_monster_stat(player_type* attacker_ptr, player_attack_type* pa_ptr, const POSITION y, const POSITION x, int* num);

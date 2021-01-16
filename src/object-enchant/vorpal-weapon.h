#pragma once

#include "player-attack/player-attack-util.h"
#include "system/angband.h"

void process_vorpal_attack(player_type* attacker_ptr, player_attack_type* pa_ptr, const bool vorpal_cut, const int vorpal_chance);

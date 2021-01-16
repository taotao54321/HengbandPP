﻿#pragma once

#include "combat/combat-options-type.h"
#include "monster-attack/monster-attack-util.h"
#include "player-attack/player-attack-util.h"
#include "system/angband.h"

MULTIPLY mult_hissatsu(player_type* attacker_ptr, MULTIPLY mult, BIT_FLAGS* flgs, monster_type* m_ptr, combat_options mode);
void concentration(player_type* creature_ptr);
bool choose_kata(player_type* creature_ptr);
int calc_attack_quality(player_type* attacker_ptr, player_attack_type* pa_ptr);
void mineuchi(player_type* attacker_ptr, player_attack_type* pa_ptr);
void musou_counterattack(player_type* target_ptr, monap_type* monap_ptr);

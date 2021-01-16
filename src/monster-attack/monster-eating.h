﻿#pragma once

#include "monster-attack/monster-attack-util.h"
#include "system/angband.h"

void process_eat_gold(player_type* target_ptr, monap_type* monap_ptr);
bool check_eat_item(player_type* target_ptr, monap_type* monap_ptr);
void process_eat_item(player_type* target_ptr, monap_type* monap_ptr);
void process_eat_food(player_type* target_ptr, monap_type* monap_ptr);
void process_eat_lite(player_type* target_ptr, monap_type* monap_ptr);

bool process_un_power(player_type* target_ptr, monap_type* monap_ptr);
bool check_drain_hp(player_type* target_ptr, const s32b d);
void process_drain_life(player_type* target_ptr, monap_type* monap_ptr, const bool resist_drain);
void process_drain_mana(player_type* target_ptr, monap_type* monap_ptr);

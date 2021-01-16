﻿#pragma once

#include "system/angband.h"

bool project_all_los(player_type* caster_ptr, EFFECT_ID typ, HIT_POINT dam);
bool speed_monsters(player_type* caster_ptr);
bool slow_monsters(player_type* caster_ptr, int power);
bool sleep_monsters(player_type* caster_ptr, int power);
void aggravate_monsters(player_type* caster_ptr, MONSTER_IDX who);
bool banish_evil(player_type* caster_ptr, int dist);
bool turn_undead(player_type* caster_ptr);
bool dispel_evil(player_type* caster_ptr, HIT_POINT dam);
bool dispel_good(player_type* caster_ptr, HIT_POINT dam);
bool dispel_undead(player_type* caster_ptr, HIT_POINT dam);
bool dispel_monsters(player_type* caster_ptr, HIT_POINT dam);
bool dispel_living(player_type* caster_ptr, HIT_POINT dam);
bool dispel_demons(player_type* caster_ptr, HIT_POINT dam);
bool crusade(player_type* caster_ptr);
bool confuse_monsters(player_type* caster_ptr, HIT_POINT dam);
bool charm_monsters(player_type* caster_ptr, HIT_POINT dam);
bool charm_animals(player_type* caster_ptr, HIT_POINT dam);
bool stun_monsters(player_type* caster_ptr, HIT_POINT dam);
bool stasis_monsters(player_type* caster_ptr, HIT_POINT dam);
bool mindblast_monsters(player_type* caster_ptr, HIT_POINT dam);
bool banish_monsters(player_type* caster_ptr, int dist);
bool turn_evil(player_type* caster_ptr, HIT_POINT dam);
bool turn_monsters(player_type* caster_ptr, HIT_POINT dam);
bool deathray_monsters(player_type* caster_ptr);
bool probing(player_type* caster_ptr);

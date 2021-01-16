﻿#pragma once

#include "system/angband.h"

void lite_room(player_type* caster_ptr, POSITION y1, POSITION x1);
bool starlight(player_type* caster_ptr, bool magic);
void unlite_room(player_type* caster_ptr, POSITION y1, POSITION x1);
bool lite_area(player_type* caster_ptr, HIT_POINT dam, POSITION rad);
bool unlite_area(player_type* caster_ptr, HIT_POINT dam, POSITION rad);
bool lite_line(player_type* caster_ptr, DIRECTION dir, HIT_POINT dam);

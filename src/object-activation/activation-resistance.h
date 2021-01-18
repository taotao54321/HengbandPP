#pragma once

#include "system/angband.h"

bool activate_resistance_elements(player_type* user_ptr);
bool activate_resistance_acid(player_type* user_ptr, concptr name);
bool activate_resistance_elec(player_type* user_ptr, concptr name);
bool activate_resistance_fire(player_type* user_ptr, concptr name);
bool activate_resistance_cold(player_type* user_ptr, concptr name);
bool activate_resistance_pois(player_type* user_ptr, concptr name);
bool activate_acid_ball_and_resistance(player_type* user_ptr, concptr name);
bool activate_elec_ball_and_resistance(player_type* user_ptr, concptr name);
bool activate_fire_ball_and_resistance(player_type* user_ptr, concptr name);
bool activate_cold_ball_and_resistance(player_type* user_ptr, concptr name);
bool activate_pois_ball_and_resistance(player_type* user_ptr, concptr name);
bool activate_ultimate_resistance(player_type* user_ptr);

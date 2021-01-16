﻿#pragma once

#include "system/angband.h"

void do_cmd_pref(player_type* creature_ptr);
void do_cmd_colors(player_type* creature_ptr, void (*process_autopick_file_command)(char*));
void do_cmd_note(void);
void do_cmd_version(void);
void do_cmd_feeling(player_type* creature_ptr);
void do_cmd_time(player_type* creature_ptr);

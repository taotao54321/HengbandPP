﻿#pragma once

#include "system/angband.h"

extern char auto_dump_header[];
extern char auto_dump_footer[];

errr process_pref_file(player_type* creature_ptr, concptr name, void (*process_autopick_file_command)(char*));
errr process_autopick_file(player_type* creature_ptr, concptr name, void (*process_autopick_file_command)(char*));
errr process_histpref_file(player_type* creature_ptr, concptr name, void (*process_autopick_file_command)(char*));
bool read_histpref(player_type* creature_ptr, void (*process_autopick_file_command)(char*));

void auto_dump_printf(FILE* auto_dump_stream, concptr fmt, ...);
bool open_auto_dump(FILE** fpp, concptr buf, concptr mark);
void close_auto_dump(FILE** fpp, concptr auto_dump_mark);

void load_all_pref_files(player_type* player_ptr);

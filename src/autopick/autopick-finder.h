#pragma once

#include "autopick/autopick-util.h"
#include "system/angband.h"
#include "system/object-type-definition.h"

int find_autopick_list(player_type* player_ptr, object_type* o_ptr);
bool get_object_for_search(player_type* player_ptr, object_type** o_handle, concptr* search_strp);
bool get_destroyed_object_for_search(player_type* player_ptr, object_type** o_handle, concptr* search_strp);
byte get_string_for_search(player_type* player_ptr, object_type** o_handle, concptr* search_strp);
void search_for_object(player_type* player_ptr, text_body_type* tb, object_type* o_ptr, bool forward);
void search_for_string(text_body_type* tb, concptr search_str, bool forward);

#pragma once

#include "autopick/autopick-util.h"
#include "system/angband.h"

/*! APE means AutoPickEditor*/
typedef enum ape_quittance {
    APE_QUIT = 0,
    APE_QUIT_WITHOUT_SAVE = 1,
    APE_QUIT_AND_SAVE = 2
} ape_quittance;

ape_quittance do_editor_command(player_type* player_ptr, text_body_type* tb, int com_id);

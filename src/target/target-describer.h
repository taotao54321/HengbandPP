#pragma once

#include "system/angband.h"
#include "target/target-types.h"

extern bool show_gold_on_floor;

char examine_grid(player_type *subject_ptr, const POSITION y, const POSITION x, target_type mode, concptr info);

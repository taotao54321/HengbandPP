﻿#pragma once

#include "system/angband.h"

typedef struct bmc_type bmc_type;
bool cast_blue_scare(player_type* caster_ptr, bmc_type* bmc_ptr);
bool cast_blue_blind(player_type* caster_ptr, bmc_type* bmc_ptr);
bool cast_blue_confusion(player_type* caster_ptr, bmc_type* bmc_ptr);
bool cast_blue_slow(player_type* caster_ptr, bmc_type* bmc_ptr);
bool cast_blue_sleep(player_type* caster_ptr, bmc_type* bmc_ptr);

﻿#pragma once

#include "info-reader/info-reader-util.h"
#include "info-reader/parse-error-types.h"
#include "system/angband.h"

errr parse_fixed_map(player_type* player_ptr, concptr name, int ymin, int xmin, int ymax, int xmax);

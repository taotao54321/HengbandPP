﻿#pragma once

#include "system/angband.h"
#include "system/object-type-definition.h"

// Item Throw.
typedef struct grid_type grid_type;
typedef struct monster_type monster_type;
typedef struct it_type {
    int mult;
    bool boomerang;
    OBJECT_IDX shuriken;
    OBJECT_IDX item;
    POSITION y;
    POSITION x;
    POSITION ty;
    POSITION tx;
    POSITION prev_y;
    POSITION prev_x;
    POSITION ny[19];
    POSITION nx[19];
    int chance;
    int tdam;
    int tdis;
    int cur_dis;
    int visible;
    PERCENTAGE corruption_possibility;
    object_type* q_ptr;
    object_type* o_ptr;
    bool hit_body;
    bool hit_wall;
    bool equiped_item;
    bool return_when_thrown;
    GAME_TEXT o_name[MAX_NLEN];
    int msec;
    BIT_FLAGS obj_flags[TR_FLAG_SIZE];
    bool come_back;
    bool do_drop;
    grid_type* g_ptr;
    monster_type* m_ptr;
    GAME_TEXT m_name[MAX_NLEN];
    int back_chance;
    char o2_name[MAX_NLEN];
    bool super_boomerang;
} it_type;

it_type* initialize_it_type(
    it_type* item_throw_ptr, object_type* q_ptr, const int delay_factor_val, const int mult, const bool boomerang, const OBJECT_IDX shuriken);

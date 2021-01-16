﻿#pragma once

typedef enum dungeon_flag_type {
    DF1_WINNER = 0x00000001L,
    DF1_MAZE = 0x00000002L,
    DF1_SMALLEST = 0x00000004L,
    DF1_BEGINNER = 0x00000008L,
    DF1_BIG = 0x00000010L,
    DF1_NO_DOORS = 0x00000020L,
    DF1_WATER_RIVER = 0x00000040L,
    DF1_LAVA_RIVER = 0x00000080L,
    DF1_CURTAIN = 0x00000100L,
    DF1_GLASS_DOOR = 0x00000200L,
    DF1_CAVE = 0x00000400L,
    DF1_CAVERN = 0x00000800L,
    DF1_ARCADE = 0x00001000L,
    DF1_LAKE_ACID = 0x00002000L,
    DF1_LAKE_POISONOUS = 0x00004000L,
    DF1_XXX15 = 0x00008000L,
    DF1_FORGET = 0x00010000L,
    DF1_LAKE_WATER = 0x00020000L,
    DF1_LAKE_LAVA = 0x00040000L,
    DF1_LAKE_RUBBLE = 0x00080000L,
    DF1_LAKE_TREE = 0x00100000L,
    DF1_NO_VAULT = 0x00200000L,
    DF1_ARENA = 0x00400000L,
    DF1_DESTROY = 0x00800000L,
    DF1_GLASS_ROOM = 0x01000000L,
    DF1_NO_CAVE = 0x02000000L,
    DF1_NO_MAGIC = 0x04000000L,
    DF1_NO_MELEE = 0x08000000L,
    DF1_CHAMELEON = 0x10000000L,
    DF1_DARKNESS = 0x20000000L,
    DF1_ACID_RIVER = 0x40000000L,
    DF1_POISONOUS_RIVER = 0x80000000L,
} dungeon_flag_type;

#define DF1_LAKE_MASK (DF1_LAKE_WATER | DF1_LAKE_LAVA | DF1_LAKE_RUBBLE | DF1_LAKE_TREE | DF1_LAKE_POISONOUS | DF1_LAKE_ACID)

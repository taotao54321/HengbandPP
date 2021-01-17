﻿#pragma once

#include "system/angband.h"
#include "system/object-type-definition.h"

/*!
 * @struct artifact_type
 * @brief 固定アーティファクト情報の構造体 / Artifact structure.
 * @details
 * @note
 * the save-file only writes "cur_num" to the savefile.
 * "max_num" is always "1" (if that artifact "exists")
 */
typedef struct artifact_type {
    STR_OFFSET name;           /*!< アーティファクト名(headerオフセット参照) / Name (offset) */
    STR_OFFSET text;           /*!< アーティファクト解説(headerオフセット参照) / Text (offset) */
    tval_type tval;            /*!< ベースアイテム大項目ID / Artifact type */
    OBJECT_SUBTYPE_VALUE sval; /*!< ベースアイテム小項目ID / Artifact sub type */
    PARAMETER_VALUE pval;      /*!< pval修正値 / Artifact extra info */
    HIT_PROB to_h;             /*!< 命中ボーナス値 /  Bonus to hit */
    HIT_POINT to_d;            /*!< ダメージボーナス値 / Bonus to damage */
    ARMOUR_CLASS to_a;         /*!< ACボーナス値 / Bonus to armor */
    ARMOUR_CLASS ac;           /*!< 上書きベースAC値 / Base armor */
    DICE_NUMBER dd;
    DICE_SID ds;                   /*!< ダイス値 / Damage when hits */
    WEIGHT weight;                 /*!< 重量 / Weight */
    PRICE cost;                    /*!< 基本価格 / Artifact "cost" */
    BIT_FLAGS flags[TR_FLAG_SIZE]; /*! アイテムフラグ / Artifact Flags */
    BIT_FLAGS gen_flags;           /*! アイテム生成フラグ / flags for generate */
    DEPTH level;                   /*! 基本生成階 / Artifact level */
    RARITY rarity;                 /*! レアリティ / Artifact rarity */
    byte cur_num;                  /*! 現在の生成数 / Number created (0 or 1) */
    byte max_num;                  /*! (未使用)最大生成数 / Unused (should be "1") */
    FLOOR_IDX floor_id;            /*! アイテムを落としたフロアのID / Leaved on this location last time */
    byte act_idx;                  /*! 発動能力ID / Activative ability index */
} artifact_type;

extern artifact_type* a_info;
extern char* a_name;
extern char* a_text;
extern ARTIFACT_IDX max_a_idx;

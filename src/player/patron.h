﻿#pragma once

#include "system/angband.h"

#define MAX_PATRON 16 /*!< カオスパトロンの最大定義数 / The number of "patrons" available (for Chaos Warriors) */

/* カオスパトロンからの報酬種別定義 / Chaos Warrior: Reward types: */
#define REW_POLY_SLF 1  /*!< カオスパトロンからの報酬: 自己変容 */
#define REW_GAIN_EXP 2  /*!< カオスパトロンからの報酬: 経験値増加 */
#define REW_LOSE_EXP 3  /*!< カオスパトロンからの報酬: 経験値減少 */
#define REW_GOOD_OBJ 4  /*!< カオスパトロンからの報酬: GOODなアイテム単体の下賜 */
#define REW_GREA_OBJ 5  /*!< カオスパトロンからの報酬: GREATなアイテム単体の下賜 */
#define REW_CHAOS_WP 6  /*!< カオスパトロンからの報酬: 混沌武器の下賜 */
#define REW_GOOD_OBS 7  /*!< カオスパトロンからの報酬: GOODなアイテム複数の下賜 */
#define REW_GREA_OBS 8  /*!< カオスパトロンからの報酬: GREATなアイテム複数の下賜 */
#define REW_TY_CURSE 9  /*!< カオスパトロンからの報酬: 太古の怨念 */
#define REW_SUMMON_M 10 /*!< カオスパトロンからの報酬: 敵対的なモンスターの召喚(通常) */
#define REW_H_SUMMON 11 /*!< カオスパトロンからの報酬: 敵対的なモンスターの召喚(hi-summon) */
#define REW_DO_HAVOC 12 /*!< カオスパトロンからの報酬: 混沌招来 */
#define REW_GAIN_ABL 13 /*!< カオスパトロンからの報酬: 増強 */
#define REW_LOSE_ABL 14 /*!< カオスパトロンからの報酬: 1能力低下 */
#define REW_RUIN_ABL 15 /*!< カオスパトロンからの報酬: 全能力低下 */
#define REW_AUGM_ABL 16 /*!< カオスパトロンからの報酬: 1能力上昇 */
#define REW_POLY_WND 17 /*!< カオスパトロンからの報酬: 傷の変化 */
#define REW_HEAL_FUL 18 /*!< カオスパトロンからの報酬: 完全回復 */
#define REW_HURT_LOT 19 /*!< カオスパトロンからの報酬: 分解の球によるダメージ */
#define REW_CURSE_WP 20 /*!< カオスパトロンからの報酬: 武器呪縛 */
#define REW_CURSE_AR 21 /*!< カオスパトロンからの報酬: 防具呪縛 */
#define REW_PISS_OFF 22 /*!< カオスパトロンからの報酬: 苛立ち */
#define REW_WRATH 23    /*!< カオスパトロンからの報酬: 怒り */
#define REW_DESTRUCT 24 /*!< カオスパトロンからの報酬: *破壊* */
#define REW_GENOCIDE 25 /*!< カオスパトロンからの報酬: シンボル抹殺 */
#define REW_MASS_GEN 26 /*!< カオスパトロンからの報酬: 周辺抹殺 */
#define REW_DISPEL_C 27 /*!< カオスパトロンからの報酬: モンスター退散 */
#define REW_UNUSED_1 28 /*!< カオスパトロンからの報酬: 未使用 */
#define REW_UNUSED_2 29 /*!< カオスパトロンからの報酬: 未使用 */
#define REW_UNUSED_3 30 /*!< カオスパトロンからの報酬: 未使用 */
#define REW_UNUSED_4 31 /*!< カオスパトロンからの報酬: 未使用 */
#define REW_UNUSED_5 32 /*!< カオスパトロンからの報酬: 未使用 */
#define REW_IGNORE 33   /*!< カオスパトロンからの報酬: 無視 */
#define REW_SER_UNDE 34 /*!< カオスパトロンからの報酬: アンデッドの下僕下賜 */
#define REW_SER_DEMO 35 /*!< カオスパトロンからの報酬: 悪魔の下僕下賜 */
#define REW_SER_MONS 36 /*!< カオスパトロンからの報酬: モンスターの下僕下賜 */

extern const concptr chaos_patrons[MAX_PATRON];

void gain_level_reward(player_type* creature_ptr, int chosen_reward);
void admire_from_patron(player_type* creature_ptr);

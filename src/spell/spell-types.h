﻿#pragma once

typedef enum spells_type {
    GF_NONE = 0,
    GF_ELEC = 1,            /*!< 魔法効果: 電撃*/
    GF_POIS = 2,            /*!< 魔法効果: 毒*/
    GF_ACID = 3,            /*!< 魔法効果: 酸*/
    GF_COLD = 4,            /*!< 魔法効果: 冷気*/
    GF_FIRE = 5,            /*!< 魔法効果: 火炎*/
    GF_PSY_SPEAR = 9,       /*!< 魔法効果: 光の剣*/
    GF_MISSILE = 10,        /*!< 魔法効果: 弱魔力*/
    GF_ARROW = 11,          /*!< 魔法効果: 射撃*/
    GF_PLASMA = 12,         /*!< 魔法効果: プラズマ*/
    GF_WATER = 14,          /*!< 魔法効果: 水流*/
    GF_LITE = 15,           /*!< 魔法効果: 閃光*/
    GF_DARK = 16,           /*!< 魔法効果: 暗黒*/
    GF_LITE_WEAK = 17,      /*!< 魔法効果: 弱光*/
    GF_DARK_WEAK = 18,      /*!< 魔法効果: 弱暗*/
    GF_SHARDS = 20,         /*!< 魔法効果: 破片*/
    GF_SOUND = 21,          /*!< 魔法効果: 轟音*/
    GF_CONFUSION = 22,      /*!< 魔法効果: 混乱*/
    GF_FORCE = 23,          /*!< 魔法効果: フォース*/
    GF_INERTIAL = 24,       /*!< 魔法効果: 遅鈍*/
    GF_MANA = 26,           /*!< 魔法効果: 純粋魔力*/
    GF_METEOR = 27,         /*!< 魔法効果: 隕石*/
    GF_ICE = 28,            /*!< 魔法効果: 極寒*/
    GF_CHAOS = 30,          /*!< 魔法効果: カオス*/
    GF_NETHER = 31,         /*!< 魔法効果: 地獄*/
    GF_DISENCHANT = 32,     /*!< 魔法効果: 劣化*/
    GF_NEXUS = 33,          /*!< 魔法効果: 因果混乱*/
    GF_TIME = 34,           /*!< 魔法効果: 時間逆転*/
    GF_GRAVITY = 35,        /*!< 魔法効果: 重力*/
    GF_KILL_WALL = 40,      /*!< 魔法効果: 岩石溶解*/
    GF_KILL_DOOR = 41,      /*!< 魔法効果: ドア破壊*/
    GF_KILL_TRAP = 42,      /*!< 魔法効果: トラップ破壊*/
    GF_MAKE_WALL = 45,      /*!< 魔法効果: 壁生成*/
    GF_MAKE_DOOR = 46,      /*!< 魔法効果: ドア生成*/
    GF_MAKE_TRAP = 47,      /*!< 魔法効果: トラップ生成*/
    GF_MAKE_TREE = 48,      /*!< 魔法効果: 森林生成*/
    GF_OLD_CLONE = 51,      /*!< 魔法効果: クローン・モンスター*/
    GF_OLD_POLY = 52,       /*!< 魔法効果: チェンジ・モンスター*/
    GF_OLD_HEAL = 53,       /*!< 魔法効果: 回復モンスター*/
    GF_OLD_SPEED = 54,      /*!< 魔法効果: スピード・モンスター*/
    GF_OLD_SLOW = 55,       /*!< 魔法効果: スロウ・モンスター*/
    GF_OLD_CONF = 56,       /*!< 魔法効果: パニック・モンスター*/
    GF_OLD_SLEEP = 57,      /*!< 魔法効果: スリープ・モンスター*/
    GF_HYPODYNAMIA = 58,    /*!< 魔法効果: 衰弱*/
    GF_AWAY_UNDEAD = 61,    /*!< 魔法効果: アンデッド・アウェイ*/
    GF_AWAY_EVIL = 62,      /*!< 魔法効果: 邪悪飛ばし*/
    GF_AWAY_ALL = 63,       /*!< 魔法効果: テレポート・アウェイ*/
    GF_TURN_UNDEAD = 64,    /*!< 魔法効果: アンデッド恐慌*/
    GF_TURN_EVIL = 65,      /*!< 魔法効果: 邪悪恐慌*/
    GF_TURN_ALL = 66,       /*!< 魔法効果: モンスター恐慌*/
    GF_DISP_UNDEAD = 67,    /*!< 魔法効果: アンデッド退散*/
    GF_DISP_EVIL = 68,      /*!< 魔法効果: 邪悪退散*/
    GF_DISP_ALL = 69,       /*!< 魔法効果: モンスター退散*/
    GF_DISP_DEMON = 70,     /*!< 魔法効果: 悪魔退散*/
    GF_DISP_LIVING = 71,    /*!< 魔法効果: 生命退散*/
    GF_ROCKET = 72,         /*!< 魔法効果: ロケット*/
    GF_NUKE = 73,           /*!< 魔法効果: 放射性廃棄物*/
    GF_MAKE_GLYPH = 74,     /*!< 魔法効果: 守りのルーン生成*/
    GF_STASIS = 75,         /*!< 魔法効果: モンスター拘束*/
    GF_STONE_WALL = 76,     /*!< 魔法効果: 壁生成*/
    GF_DEATH_RAY = 77,      /*!< 魔法効果: 死の光線*/
    GF_STUN = 78,           /*!< 魔法効果: 朦朧*/
    GF_HOLY_FIRE = 79,      /*!< 魔法効果: 聖光*/
    GF_HELL_FIRE = 80,      /*!< 魔法効果: 地獄の劫火*/
    GF_DISINTEGRATE = 81,   /*!< 魔法効果: 分解*/
    GF_CHARM = 82,          /*!< 魔法効果: モンスター魅了*/
    GF_CONTROL_UNDEAD = 83, /*!< 魔法効果: アンデッド支配*/
    GF_CONTROL_ANIMAL = 84, /*!< 魔法効果: 動物支配*/
    GF_PSI = 85,            /*!< 魔法効果: サイキック攻撃*/
    GF_PSI_DRAIN = 86,      /*!< 魔法効果: 精神吸収*/
    GF_TELEKINESIS = 87,    /*!< 魔法効果: テレキシネス*/
    GF_JAM_DOOR = 88,       /*!< 魔法効果: 施錠*/
    GF_DOMINATION = 89,     /*!< 魔法効果: 精神支配*/
    GF_DISP_GOOD = 90,      /*!< 魔法効果: 善良退散*/
    GF_DRAIN_MANA = 91,     /*!< 魔法効果: 魔力吸収*/
    GF_MIND_BLAST = 92,     /*!< 魔法効果: 精神攻撃*/
    GF_BRAIN_SMASH = 93,    /*!< 魔法効果: 脳攻撃*/
    GF_CAUSE_1 = 94,        /*!< 魔法効果: 軽傷の呪い*/
    GF_CAUSE_2 = 95,        /*!< 魔法効果: 重傷の呪い*/
    GF_CAUSE_3 = 96,        /*!< 魔法効果: 致命傷の呪い*/
    GF_CAUSE_4 = 97,        /*!< 魔法効果: 秘孔を突く*/
    GF_HAND_DOOM = 98,      /*!< 魔法効果: 破滅の手*/
    GF_CAPTURE = 99,        /*!< 魔法効果: 捕縛*/
    GF_ANIM_DEAD = 100,     /*!< 魔法効果: 死者復活*/
    GF_CHARM_LIVING = 101,  /*!< 魔法効果: 生命魅了*/
    GF_IDENTIFY = 102,      /*!< 魔法効果: 鑑定*/
    GF_ATTACK = 103,        /*!< 魔法効果: 白兵*/
    GF_ENGETSU = 104,       /*!< 魔法効果: 円月*/
    GF_GENOCIDE = 105,      /*!< 魔法効果: 抹殺*/
    GF_PHOTO = 106,         /*!< 魔法効果: 撮影*/
    GF_CONTROL_DEMON = 107, /*!< 魔法効果: 悪魔支配*/
    GF_LAVA_FLOW = 108,     /*!< 魔法効果: 溶岩噴出*/
    GF_BLOOD_CURSE = 109,   /*!< 魔法効果: 血の呪い*/
    GF_SEEKER = 110,        /*!< 魔法効果: シーカーレイ*/
    GF_SUPER_RAY = 111,     /*!< 魔法効果: スーパーレイ*/
    GF_STAR_HEAL = 112,     /*!< 魔法効果: 星の癒し*/
    GF_WATER_FLOW = 113,    /*!< 魔法効果: 流水*/
    GF_CRUSADE = 114,       /*!< 魔法効果: 聖戦*/
    GF_STASIS_EVIL = 115,   /*!< 魔法効果: 邪悪拘束*/
    GF_WOUNDS = 116,        /*!< 魔法効果: 創傷*/
    MAX_GF = 117,           /*!< 欠番を無視した最大サイズ (直上の値+1) */
} spells_type;

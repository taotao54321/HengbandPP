#pragma once

typedef enum smart_learn_type {
	SM_RES_ACID = 0x00000001, /*!< モンスターの学習フラグ: プレイヤーに酸耐性あり */
    SM_RES_ELEC = 0x00000002, /*!< モンスターの学習フラグ: プレイヤーに電撃耐性あり */
    SM_RES_FIRE = 0x00000004, /*!< モンスターの学習フラグ: プレイヤーに火炎耐性あり */
    SM_RES_COLD = 0x00000008, /*!< モンスターの学習フラグ: プレイヤーに冷気耐性あり */
    SM_RES_POIS = 0x00000010, /*!< モンスターの学習フラグ: プレイヤーに毒耐性あり */
    SM_RES_NETH = 0x00000020, /*!< モンスターの学習フラグ: プレイヤーに地獄耐性あり */
    SM_RES_LITE = 0x00000040, /*!< モンスターの学習フラグ: プレイヤーに閃光耐性あり */
    SM_RES_DARK = 0x00000080, /*!< モンスターの学習フラグ: プレイヤーに暗黒耐性あり */
    SM_RES_FEAR = 0x00000100, /*!< モンスターの学習フラグ: プレイヤーに恐怖耐性あり */
    SM_RES_CONF = 0x00000200, /*!< モンスターの学習フラグ: プレイヤーに混乱耐性あり */
    SM_RES_CHAOS = 0x00000400, /*!< モンスターの学習フラグ: プレイヤーにカオス耐性あり */
    SM_RES_DISEN = 0x00000800, /*!< モンスターの学習フラグ: プレイヤーに劣化耐性あり */
    SM_RES_BLIND = 0x00001000, /*!< モンスターの学習フラグ: プレイヤーに盲目耐性あり */
    SM_RES_NEXUS = 0x00002000, /*!< モンスターの学習フラグ: プレイヤーに因果混乱耐性あり */
    SM_RES_SOUND = 0x00004000, /*!< モンスターの学習フラグ: プレイヤーに轟音耐性あり */
    SM_RES_SHARD = 0x00008000, /*!< モンスターの学習フラグ: プレイヤーに破片耐性あり */
    SM_OPP_ACID = 0x00010000, /*!< モンスターの学習フラグ: プレイヤーに二重酸耐性あり */
    SM_OPP_ELEC = 0x00020000, /*!< モンスターの学習フラグ: プレイヤーに二重電撃耐性あり */
    SM_OPP_FIRE = 0x00040000, /*!< モンスターの学習フラグ: プレイヤーに二重火炎耐性あり */
    SM_OPP_COLD = 0x00080000, /*!< モンスターの学習フラグ: プレイヤーに二重冷気耐性あり */
    SM_OPP_POIS = 0x00100000, /*!< モンスターの学習フラグ: プレイヤーに二重毒耐性あり */
    SM_OPP_XXX1 = 0x00200000, /*!< 未使用 / (unused) */
    SM_CLONED = 0x00400000, /*!< クローンである / Cloned */
    SM_PET = 0x00800000, /*!< ペットである / Pet */
    SM_IMM_ACID = 0x01000000, /*!< モンスターの学習フラグ: プレイヤーに酸免疫あり */
    SM_IMM_ELEC = 0x02000000, /*!< モンスターの学習フラグ: プレイヤーに電撃免疫あり */
    SM_IMM_FIRE = 0x04000000, /*!< モンスターの学習フラグ: プレイヤーに火炎免疫あり */
    SM_IMM_COLD = 0x08000000, /*!< モンスターの学習フラグ: プレイヤーに冷気免疫あり */
    SM_FRIENDLY = 0x10000000, /*!< 友好的である / Friendly */
    SM_IMM_REFLECT = 0x20000000, /*!< モンスターの学習フラグ: プレイヤーに反射あり */
    SM_IMM_FREE = 0x40000000, /*!< モンスターの学習フラグ: プレイヤーに麻痺耐性あり */
    SM_IMM_MANA = 0x80000000, /*!< モンスターの学習フラグ: プレイヤーにMPがない */
} smart_learn_type;

﻿#include "angband.h"
#include "player-race.h"

/*!
 * @brief 種族情報 /
 * Player Races
 * @details
 * <pre>
 *      Title,
 *      {STR,INT,WIS,DEX,CON,CHR},
 *      r_dis, r_dev, r_sav, r_stl, r_srh, r_fos, r_thn, r_thb,
 *      hitdie, exp base,
 *      Age (Base, Mod),
 *      Male (Hgt, Wgt),
 *      Female (Hgt, Wgt)
 *      infra,
 *      class-choices
 * </pre>
 */
const player_race race_info[MAX_RACES] =
{
	{
#ifdef JP
		"人間",
#endif
		"Human",

		{  0,  0,  0,  0,  0,  0 },
		0,  0,  0,  0,  0,  10,  0,  0,
		10,  100,
		14,  6,
		72,  6, 180, 25,
		66,  4, 150, 20,
		0,
		0xFFFFFFF,
	},
	{
#ifdef JP
		"ハーフエルフ",
#endif
		"Half-Elf",

		{ -1,  1,  1,  1, -1,  1 },
		2,  3,  2,  1, 6,  11, -1,  5,
		9,  110,
		24, 16,
		66,  6, 130, 15,
		62,  6, 100, 10,
		2,
		0xE77E7FF,
	},
	{
#ifdef JP
		"エルフ",
#endif
		"Elf",

		{ -1,  2,  0,  1, -2,  2 },
		5,  6,  4,  2, 8,  12, -5, 15,
		8,  120,
		75, 75,
		60,  4, 100,  6,
		54,  4, 80,  6,
		3,
		0xE77E75B,

	},
	{
#ifdef JP
		"ホビット",
#endif
		"Hobbit",

		{ -2,  1,  1,  3,  2,  1 },
		15, 12, 10, 5, 12,  15, -10, 20,
		7,  110,
		21, 12,
		36,  3, 60,  3,
		33,  3, 50,  3,
		4,
		0xF6FFC0B,
	},
	{
#ifdef JP
		"ノーム",
#endif
		"Gnome",

		{ -1,  2,  0,  2,  1, -1 },
		10, 8, 7,  3, 6,  13, -8, 12,
		8,  120,
		50, 40,
		42,  3, 90,  6,
		39,  3, 75,  3,
		4,
		0xF67D60F,
	},
	{
#ifdef JP
		"ドワーフ",
#endif
		"Dwarf",

		{  2, -2,  2, -2,  2, -1 },
		2,  7,  6,  -1,  7,  10, 15,  0,
		11,  125,
		35, 15,
		48,  3, 150, 10,
		46,  3, 120, 10,
		5,
		0x1890005,
	},
	{
#ifdef JP
		"ハーフオーク",
#endif
		"Half-Orc",

		{  2, -1,  0,  0,  1, -2 },
		-3, -3, -2,  -1,  0, 7, 12, -5,
		10,  110,
		11,  4,
		66,  1, 150,  5,
		62,  1, 120,  5,
		3,
		0xDD8818D,
	},
	{
#ifdef JP
		"ハーフトロル",
#endif
		"Half-Troll",

		{ 4, -4, -1, -3,  3, -3 },
		-5, -8, -5, -2,  -1, 5, 20, -10,
		12,  125,
		20, 10,
		96, 10, 250, 50,
		84,  8, 225, 40,
		3,
		0x0880005,
	},
	{
#ifdef JP
		"アンバライト",
#endif
		"Amberite",

		{  1,  2,  2,  2,  3,  2 },
		4,  5,  3,  2, 3, 13, 15, 10,
		10,  225,
		50, 50,
		82, 5, 190, 20,
		78,  6, 180, 15,
		0,
		0xFFFF7FF,
	},
	{
#ifdef JP
		"ハイエルフ",
#endif
		"High-Elf",

		{  1,  3,  -1,  3,  1,  3 },
		4,  13, 12,  4,  3, 14, 10, 25,
		10,  200,
		100, 30,
		90, 10, 190, 20,
		82, 10, 180, 15,
		4,
		0xF77E75B,
	},
	{
#ifdef JP
		"野蛮人",
#endif
		"Barbarian",

		{ 3, -2,  -1,  1,  2, 0 },
		-2, -10, 2,  -1,  1, 7, 12, 10,
		11, 120,
		14, 8,
		82, 5, 200, 20,
		78,  6, 190, 15,
		0,
		0x5C0A09D,
	},
	{
#ifdef JP
		"ハーフオーガ",
#endif
		"Half-Ogre",

		{ 3, -2, 0, -1, 3, -2 },
		-3, -5, -3, -2, -1, 5, 20, 0,
		12,  145,
		40, 10,
		92, 10, 255, 60,
		80,  8, 235, 60,
		3,
		0x0A80407,
	},
	{
#ifdef JP
		"半巨人",
#endif
		"Half-Giant",

		{ 4, -2, -2, -2, 3, -2 },
		-6, -8, -3, -2, -1, 5, 25, 5,
		13, 160,
		40, 10,
		100,10, 255, 65,
		80, 10, 240, 64,
		3,
		0x8880011,
	},
	{
#ifdef JP
		"半タイタン",
#endif
		"Half-Titan",

		{ 5, 1, 2, -2, 3, 1 },
		-5, 5, 1, -2, 1, 8, 25, 0,
		14, 255,
		100,30,
		111, 11, 255, 86,
		99, 11, 250, 86,
		0,
		0x23D4727,
	},
	{
#ifdef JP
		"サイクロプス",
#endif
		"Cyclops",

		{ 4, -3, -2, -3, 4, -3 },
		-4, -5, -3, -2, -2, 5, 20, 12,
		13, 150,
		50, 24,
		92, 10, 255, 60,
		80,  8, 235, 60,
		1,
		0x0888005,
	},
	{
#ifdef JP
		"イーク",
#endif
		"Yeek",

		{ -2, 1, 1, 1, -2, -4 },
		2, 4, 6, 3, 5, 15, -5, -5,
		7, 100,
		14, 3,
		50,  3, 90,  6,
		50,  3, 75,  3,
		2,
		0x667360F,
	},
	{
#ifdef JP
		"クラッコン",
#endif
		"Klackon",

		{ 2, -1, -1, 1, 2, -1 },
		10, 5, 3, 0, -1, 10, 5, 5,
		12, 190,
		20, 3,
		60,  3, 80,  4,
		54,  3, 70,  4,
		2,
		0x04D8011,
	},
	{
#ifdef JP
		"コボルド",
#endif
		"Kobold",

		{ 1, -1, 0, 1, 0, -2 },
		-2, -3, -1, -1, 1, 8, 10, -8,
		9, 125,
		11,  3,
		60,  1, 130,  5,
		55,  1, 100,  5,
		3,
		0x444A009,
	},
	{
#ifdef JP
		"ニーベルング",
#endif
		"Nibelung",

		{ 1, -1, 2, 0, 2, -2 },
		3, 5, 6, 1, 5, 10, 9, 0,
		11, 170,
		40, 12,
		43,  3, 92,  6,
		40,  3, 78,  3,
		5,
		0x569040F,
	},
	{
#ifdef JP
		"ダークエルフ",
#endif
		"Dark-Elf",

		{ -1, 3, 2, 2, -2, 1 },
		5, 10, 12, 3, 8, 12, -5, 10,
		9, 150,
		75, 75,
		60,  4, 100,  6,
		54,  4, 80,  6,
		5,
		0xE77C7DF,
	},
	{
#ifdef JP
		"ドラコニアン",
#endif
		"Draconian",

		{ 2, 1, 1, 1, 2, -1 },
		-2, 5, 2, 0, 1, 10, 5, 5,
		11, 220,
		75, 33,
		76,  1, 160,  5,
		72,  1, 130,  5,
		2,
		0x7FFE757,
	},
	{
#ifdef JP
		"マインドフレア",
#endif
		"Mindflayer",

		{ -3, 4, 4, 0, -2, -3 },
		10, 15, 9, 2, 5, 12, -10, -5,
		9, 140,
		100, 25,
		68,  6, 142, 15,
		63,  6, 112, 10,
		4,
		0x2334746,
	},
	{
#ifdef JP
		"インプ",
#endif
		"Imp",

		{ 0, -1, -1, 1, 2, -1 },
		-3, 2, -1, 1, -1, 10, 5, -5,
		10, 115,
		13,  4,
		68,  1, 150,  5,
		64,  1, 120,  5,
		3,
		0xDB537CB,
	},
	{
#ifdef JP
		"ゴーレム",
#endif
		"Golem",

		{ 4, -5, -5, -2, 4, -2 },
		-5, -5, 6, -1, -1, 8, 20, 0,
		12, 200,
		1, 100,
		66,  1, 200,  6,
		62,  1, 180,  6,
		4,
		0x0800001,
	},
	{
#ifdef JP
		"骸骨",
#endif
		"Skeleton",

		{ 0, 1, -2, 0, 1, -2 },
		-5, 0, 3, -1, -1, 8, 10, 0,
		10, 145,
		100, 35,
		72,  6, 50, 5,
		66,  4, 50, 5,
		2,
		0x234070F,
	},
	{
#ifdef JP
		"ゾンビ",
#endif
		"Zombie",

		{ 2, -6, -6, 1, 4, -3 },
		-5, -5, 5, -1, -1, 5, 15, 0,
		13, 150,
		100, 30,
		72, 6, 100, 25,
		66, 4, 100, 20,
		2,
		0x0800001,
	},
	{
#ifdef JP
		"吸血鬼",
#endif
		"Vampire",

		{ 3, 3, -1, -1, 1, 2 },
		4, 8, 6, 4, 1, 8, 5, 0,
		11, 200,
		100, 30,
		72,  6, 180, 25,
		66,  4, 150, 20,
		5,
		0x67DC7FF,
	},
	{
#ifdef JP
		"幽霊",
#endif
		"Spectre",

		{ -5, 4, -1, 2, 0, -3 },
		10, 15, 12, 5, 5, 14, -15, -5,
		7, 210,
		100, 30,
		72, 6, 100, 25,
		66, 4, 100, 20,
		5,
		0x631474A,
	},
	{
#ifdef JP
		"妖精",
#endif
		"Sprite",

		{ -4, 3, 3, 3, -2, 2 },
		10, 8, 6, 4, 10, 10, -12, 0,
		7, 145,
		50, 25,
		32,  2, 75,  2,
		29,  2, 65,  2,
		4,
		0x623F65E,
	},
	{
#ifdef JP
		"獣人",
#endif
		"Beastman",

		{ 2, -2, -1, -1, 2, -2 },
		-5, -2, -1, -1, -1, 5, 12, 5,
		11, 140,
		14, 6,
		65,  6, 150, 20,
		61,  6, 120, 15,
		0,
		0x57887CF,
	},
	{
#ifdef JP
		"エント",
#endif
		"Ent",
		{ 2,  0,  2, -3,  2,  0 },
		 -5,  2,  5,  -1, 0, 9,  15, -5,
		 12, 140,
		120,  60,
		111, 11, 255, 50,
		99, 11, 250, 45,
		  0,
		0x0010005,
	},
	{
#ifdef JP
		"アルコン",
#endif
		"Archon",

		{  2,  0,  4,  1,  2,  3 },
		0,  12,  8,  2, 2, 11, 10, 10,
		11,  235,
		150, 150,
		82, 5, 190, 20,
		78,  6, 180, 15,
		3,
		0x779F777,
	},
	{
#ifdef JP
		"バルログ",
		"Balrog",
#else
		"Balrog",
#endif

		{  4,  2,  -10,  2,  3,  -5 },
		-3,  12, 15,  -2,  1, 8, 20, 0,
		12,  250,
		255, 255,
		100,10, 255, 65,
		80, 10, 240, 64,
		5,
		0x7EDC4DB,
	},
	{
#ifdef JP
		"ドゥナダン",
#endif
		"Dunadan",

		{  1,  2,  2,  2,  3,  2 },
		4,  5,  3,  2, 3, 13, 15, 10,
		10,  180,
		50, 20,
		82, 5, 190, 20,
		78,  6, 180, 15,
		0,
		0xFFFF7FF,
	},
	{
#ifdef JP
		"影フェアリー",
#endif
		"Shadow-Fairy",
		{-2,  2,  2,  1, -1,  0 },
		  7,  8,  0,  6, 12, 15, -10, -5,
		  7, 120,
		200, 100,
		 80,  8, 90, 20,
		 73,  8, 80, 15,
		  4,
		0xE33C7DF,
	},
	{
#ifdef JP
		"クター",
#endif
		"Kutar",

		{  0,  -1,  -1,  1,  2,  3 },
		-2,  5,  5,  5,  -2,  6,  0,  -5,
		11,  140,
		14,  6,
		48,  6, 150, 25,
		44,  4, 130, 20,
		0,
		0xC18B7AD,
	},
	{
#ifdef JP
		"アンドロイド",
#endif
		"Android",

		{ 4, -5, -5, 0, 4, -2 },
		0, -5, 0, -2, 3, 14, 20, 10,
		13, 200,
		1, 100,
		72, 12, 240, 64,
		66, 12, 220, 64,
		0,
		0x0800001,
	},
	{
#ifdef JP
		"マーフォーク",
#endif
		"Merfolk",

		{ -1,  0,  2,  1,  -1,  1},
			2,  3,  2,  1, 6,  11, -1,  5,
			10,  130,
			24, 16,
			66,  6, 130, 15,
			62,  6, 100, 10,
			2,
			0xE77E7FF,
	},

};

/*!
 * @brief 変身種族情報
 */
const player_race mimic_info[] =
{
	{
#ifdef JP
		"[標準形態]",
#endif
		"Default",

		{  0,  0,  0,  0,  0,  0 },
		0,  0,  0,  0,  0,  10,  0,  0,
		10,  100,
		0,  0,
		0,  0, 0, 0,
		0,  0, 0, 0,
		0,
		0x000000,
	},
	{
#ifdef JP
		"[悪魔]",
#endif
		"[Demon]",

		{  5,  3,  2,  3,  4,  -6 },
		-5,  18, 20, -2,  3,  10, 40, 20,
		12,  0,
		0,  0,
		0,  0, 0, 0,
		0,  0, 0, 0,
		5,
		0x000003,
	},
	{
#ifdef JP
		"[魔王]",
#endif
		"[Demon lord]",

		{  20,  20,  20,  20,  20,  20 },
		20,  20, 25, -2,  3,  10, 70, 40,
		14,  0,
		0,  0,
		0,  0, 0, 0,
		0,  0, 0, 0,
		20,
		0x000003,
	},
	{
#ifdef JP
		"[吸血鬼]",
#endif
		"[Vampire]",

		{ 4, 4, 1, 1, 2, 3 },
		6, 12, 8, 6, 2, 12, 30, 20,
		11,  0,
		0,  0,
		0,  0, 0, 0,
		0,  0, 0, 0,
		5,
		0x000005,
	},
};

const player_race *rp_ptr;

SYMBOL_CODE get_summon_symbol_from_player(player_type *creature_ptr)
{
	SYMBOL_CODE symbol = 'N';
	switch (creature_ptr->mimic_form)
	{
	case MIMIC_NONE:
		switch (creature_ptr->prace)
		{
		case RACE_HUMAN:
		case RACE_AMBERITE:
		case RACE_BARBARIAN:
		case RACE_BEASTMAN:
		case RACE_DUNADAN:
			symbol = 'p';
			break;
		case RACE_HALF_ELF:
		case RACE_ELF:
		case RACE_HOBBIT:
		case RACE_GNOME:
		case RACE_DWARF:
		case RACE_HIGH_ELF:
		case RACE_NIBELUNG:
		case RACE_DARK_ELF:
		case RACE_MIND_FLAYER:
		case RACE_KUTAR:
		case RACE_S_FAIRY:
			symbol = 'h';
			break;
		case RACE_HALF_ORC:
			symbol = 'o';
			break;
		case RACE_HALF_TROLL:
			symbol = 'T';
			break;
		case RACE_HALF_OGRE:
			symbol = 'O';
			break;
		case RACE_HALF_GIANT:
		case RACE_HALF_TITAN:
		case RACE_CYCLOPS:
			symbol = 'P';
			break;
		case RACE_YEEK:
			symbol = 'y';
			break;
		case RACE_KLACKON:
			symbol = 'K';
			break;
		case RACE_KOBOLD:
			symbol = 'k';
			break;
		case RACE_IMP:
			if (one_in_(13)) symbol = 'U';
			else symbol = 'u';
			break;
		case RACE_DRACONIAN:
			symbol = 'd';
			break;
		case RACE_GOLEM:
		case RACE_ANDROID:
			symbol = 'g';
			break;
		case RACE_SKELETON:
			if (one_in_(13)) symbol = 'L';
			else symbol = 's';
			break;
		case RACE_ZOMBIE:
			symbol = 'z';
			break;
		case RACE_VAMPIRE:
			symbol = 'V';
			break;
		case RACE_SPECTRE:
			symbol = 'G';
			break;
		case RACE_SPRITE:
			symbol = 'I';
			break;
		case RACE_ENT:
			symbol = '#';
			break;
		case RACE_ANGEL:
			symbol = 'A';
			break;
		case RACE_DEMON:
			symbol = 'U';
			break;
		default:
			symbol = 'p';
			break;
		}
		break;
	case MIMIC_DEMON:
		if (one_in_(13)) symbol = 'U';
		else symbol = 'u';
		break;
	case MIMIC_DEMON_LORD:
		symbol = 'U';
		break;
	case MIMIC_VAMPIRE:
		symbol = 'V';
		break;
	}
	return symbol;
}
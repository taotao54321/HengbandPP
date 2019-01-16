﻿#include "angband.h"
#include "cmd-pet.h"
#include "object-curse.h"
#include "monsterrace-hook.h"
#include "objectkind-hook.h"
#include "spells-summon.h"

void gain_level_reward(int chosen_reward)
{
	object_type *q_ptr;
	object_type forge;
	char        wrath_reason[32] = "";
	int         nasty_chance = 6;
	OBJECT_TYPE_VALUE dummy = 0;
	OBJECT_SUBTYPE_VALUE dummy2 = 0;
	int         type, effect;
	concptr        reward = NULL;
	GAME_TEXT o_name[MAX_NLEN];

	int count = 0;

	if (!chosen_reward)
	{
		if (multi_rew) return;
		else multi_rew = TRUE;
	}


	if (p_ptr->lev == 13) nasty_chance = 2;
	else if (!(p_ptr->lev % 13)) nasty_chance = 3;
	else if (!(p_ptr->lev % 14)) nasty_chance = 12;

	if (one_in_(nasty_chance))
		type = randint1(20); /* Allow the 'nasty' effects */
	else
		type = randint1(15) + 5; /* Or disallow them */

	if (type < 1) type = 1;
	if (type > 20) type = 20;
	type--;


	sprintf(wrath_reason, _("%sの怒り", "the Wrath of %s"), chaos_patrons[p_ptr->chaos_patron]);

	effect = chaos_rewards[p_ptr->chaos_patron][type];

	if (one_in_(6) && !chosen_reward)
	{
		msg_format(_("%^sは褒美としてあなたを突然変異させた。", "%^s rewards you with a mutation!"), chaos_patrons[p_ptr->chaos_patron]);
		(void)gain_random_mutation(0);
		reward = _("変異した。", "mutation");
	}
	else
	{
		switch (chosen_reward ? chosen_reward : effect)
		{

		case REW_POLY_SLF:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「汝、新たなる姿を必要とせり！」", "'Thou needst a new form, mortal!'"));

			do_poly_self();
			reward = _("変異した。", "polymorphing");
			break;

		case REW_GAIN_EXP:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「汝は良く行いたり！続けよ！」", "'Well done, mortal! Lead on!'"));

			if (p_ptr->prace == RACE_ANDROID)
			{
				msg_print(_("しかし何も起こらなかった。", "But, nothing happen."));
			}
			else if (p_ptr->exp < PY_MAX_EXP)
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
				msg_print(_("更に経験を積んだような気がする。", "You feel more experienced."));

				gain_exp(ee);
				reward = _("経験値を得た", "experience");
			}
			break;

		case REW_LOSE_EXP:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「下僕よ、汝それに値せず。」", "'Thou didst not deserve that, slave.'"));

			if (p_ptr->prace == RACE_ANDROID)
			{
				msg_print(_("しかし何も起こらなかった。", "But, nothing happen."));
			}
			else
			{
				lose_exp(p_ptr->exp / 6);
				reward = _("経験値を失った。", "losing experience");
			}
			break;

		case REW_GOOD_OBJ:
#ifdef JP
			msg_format("%sの声がささやいた:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s whispers:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			msg_print(_("「我が与えし物を賢明に使うべし。」", "'Use my gift wisely.'"));

			acquirement(p_ptr->y, p_ptr->x, 1, FALSE, FALSE, FALSE);
			reward = _("上質なアイテムを手に入れた。", "a good item");
			break;

		case REW_GREA_OBJ:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「我が与えし物を賢明に使うべし。」", "'Use my gift wisely.'"));

			acquirement(p_ptr->y, p_ptr->x, 1, TRUE, FALSE, FALSE);
			reward = _("高級品のアイテムを手に入れた。", "an excellent item");
			break;

		case REW_CHAOS_WP:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「汝の行いは貴き剣に値せり。」", "'Thy deed hath earned thee a worthy blade.'"));
			q_ptr = &forge;
			dummy = TV_SWORD;
			switch (randint1(p_ptr->lev))
			{
			case 0: case 1:
				dummy2 = SV_DAGGER;
				break;
			case 2: case 3:
				dummy2 = SV_MAIN_GAUCHE;
				break;
			case 4:
				dummy2 = SV_TANTO;
				break;
			case 5: case 6:
				dummy2 = SV_RAPIER;
				break;
			case 7: case 8:
				dummy2 = SV_SMALL_SWORD;
				break;
			case 9: case 10:
				dummy2 = SV_BASILLARD;
				break;
			case 11: case 12: case 13:
				dummy2 = SV_SHORT_SWORD;
				break;
			case 14: case 15:
				dummy2 = SV_SABRE;
				break;
			case 16: case 17:
				dummy2 = SV_CUTLASS;
				break;
			case 18:
				dummy2 = SV_WAKIZASHI;
				break;
			case 19:
				dummy2 = SV_KHOPESH;
				break;
			case 20:
				dummy2 = SV_TULWAR;
				break;
			case 21:
				dummy2 = SV_BROAD_SWORD;
				break;
			case 22: case 23:
				dummy2 = SV_LONG_SWORD;
				break;
			case 24: case 25:
				dummy2 = SV_SCIMITAR;
				break;
			case 26:
				dummy2 = SV_NINJATO;
				break;
			case 27:
				dummy2 = SV_KATANA;
				break;
			case 28: case 29:
				dummy2 = SV_BASTARD_SWORD;
				break;
			case 30:
				dummy2 = SV_GREAT_SCIMITAR;
				break;
			case 31:
				dummy2 = SV_CLAYMORE;
				break;
			case 32:
				dummy2 = SV_ESPADON;
				break;
			case 33:
				dummy2 = SV_TWO_HANDED_SWORD;
				break;
			case 34:
				dummy2 = SV_FLAMBERGE;
				break;
			case 35:
				dummy2 = SV_NO_DACHI;
				break;
			case 36:
				dummy2 = SV_EXECUTIONERS_SWORD;
				break;
			case 37:
				dummy2 = SV_ZWEIHANDER;
				break;
			case 38:
				dummy2 = SV_HAYABUSA;
				break;
			default:
				dummy2 = SV_BLADE_OF_CHAOS;
			}

			object_prep(q_ptr, lookup_kind(dummy, dummy2));
			q_ptr->to_h = 3 + randint1(dun_level) % 10;
			q_ptr->to_d = 3 + randint1(dun_level) % 10;
			one_resistance(q_ptr);
			q_ptr->name2 = EGO_CHAOTIC;
			(void)drop_near(q_ptr, -1, p_ptr->y, p_ptr->x);
			reward = _("(混沌)の武器を手に入れた。", "chaos weapon");
			break;

		case REW_GOOD_OBS:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「汝の行いは貴き報いに値せり。」", "'Thy deed hath earned thee a worthy reward.'"));

			acquirement(p_ptr->y, p_ptr->x, randint1(2) + 1, FALSE, FALSE, FALSE);
			reward = _("上質なアイテムを手に入れた。", "good items");
			break;

		case REW_GREA_OBS:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「下僕よ、汝の献身への我が惜しみ無き報いを見るがよい。」", "'Behold, mortal, how generously I reward thy loyalty.'"));

			acquirement(p_ptr->y, p_ptr->x, randint1(2) + 1, TRUE, FALSE, FALSE);
			reward = _("高級品のアイテムを手に入れた。", "excellent items");
			break;

		case REW_TY_CURSE:
#ifdef JP
			msg_format("%sの声が轟き渡った:", chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s thunders:", chaos_patrons[p_ptr->chaos_patron]);
#endif

			msg_print(_("「下僕よ、汝傲慢なり。」", "'Thou art growing arrogant, mortal.'"));

			(void)activate_ty_curse(FALSE, &count);
			reward = _("禍々しい呪いをかけられた。", "cursing");
			break;

		case REW_SUMMON_M:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「我が下僕たちよ、かの傲慢なる者を倒すべし！」", "'My pets, destroy the arrogant mortal!'"));

			for (dummy = 0; dummy < randint1(5) + 1; dummy++)
			{
				(void)summon_specific(0, p_ptr->y, p_ptr->x, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET), '\0');
			}
			reward = _("モンスターを召喚された。", "summoning hostile monsters");
			break;


		case REW_H_SUMMON:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「汝、より強き敵を必要とせり！」", "'Thou needst worthier opponents!'"));

			activate_hi_summon(p_ptr->y, p_ptr->x, FALSE);
			reward = _("モンスターを召喚された。", "summoning many hostile monsters");
			break;


		case REW_DO_HAVOC:
#ifdef JP
			msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			msg_print(_("「死と破壊こそ我が喜びなり！」", "'Death and destruction! This pleaseth me!'"));

			call_chaos();
			reward = _("カオスの力が渦巻いた。", "calling chaos");
			break;


		case REW_GAIN_ABL:
#ifdef JP
			msg_format("%sの声が鳴り響いた:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s rings out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			msg_print(_("「留まるのだ、下僕よ。余が汝の肉体を鍛えん。」", "'Stay, mortal, and let me mold thee.'"));

			if (one_in_(3) && !(chaos_stats[p_ptr->chaos_patron] < 0))
				do_inc_stat(chaos_stats[p_ptr->chaos_patron]);
			else
				do_inc_stat(randint0(6));
			reward = _("能力値が上がった。", "increasing a stat");
			break;


		case REW_LOSE_ABL:
#ifdef JP
			msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			msg_print(_("「下僕よ、余は汝に飽みたり。」", "'I grow tired of thee, mortal.'"));

			if (one_in_(3) && !(chaos_stats[p_ptr->chaos_patron] < 0))
				do_dec_stat(chaos_stats[p_ptr->chaos_patron]);
			else
				(void)do_dec_stat(randint0(6));
			reward = _("能力値が下がった。", "decreasing a stat");
			break;


		case REW_RUIN_ABL:

#ifdef JP
			msg_format("%sの声が轟き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s thunders:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			msg_print(_("「汝、謙虚たることを学ぶべし！」", "'Thou needst a lesson in humility, mortal!'"));
			msg_print(_("あなたは以前より弱くなった！", "You feel less powerful!"));

			for (dummy = 0; dummy < A_MAX; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint1(15), TRUE);
			}
			reward = _("全能力値が下がった。", "decreasing all stats");
			break;

		case REW_POLY_WND:

			msg_format(_("%sの力が触れるのを感じた。", "You feel the power of %s touch you."),
				chaos_patrons[p_ptr->chaos_patron]);
			do_poly_wounds();
			reward = _("傷が変化した。", "polymorphing wounds");
			break;

		case REW_AUGM_ABL:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);

			msg_print(_("「我がささやかなる賜物を受けとるがよい！」", "'Receive this modest gift from me!'"));

			for (dummy = 0; dummy < A_MAX; dummy++)
			{
				(void)do_inc_stat(dummy);
			}
			reward = _("全能力値が上がった。", "increasing all stats");
			break;

		case REW_HURT_LOT:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「苦しむがよい、無能な愚か者よ！」", "'Suffer, pathetic fool!'"));

			fire_ball(GF_DISINTEGRATE, 0, p_ptr->lev * 4, 4);
			take_hit(DAMAGE_NOESCAPE, p_ptr->lev * 4, wrath_reason, -1);
			reward = _("分解の球が発生した。", "generating disintegration ball");
			break;

		case REW_HEAL_FUL:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			(void)restore_level();
			(void)restore_all_status();
			(void)true_healing(5000);
			reward = _("体力が回復した。", "healing");
			break;

		case REW_CURSE_WP:

			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) break;
			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「汝、武器に頼ることなかれ。」", "'Thou reliest too much on thy weapon.'"));

			dummy = INVEN_RARM;
			if (buki_motteruka(INVEN_LARM))
			{
				dummy = INVEN_LARM;
				if (buki_motteruka(INVEN_RARM) && one_in_(2)) dummy = INVEN_RARM;
			}
			object_desc(o_name, &inventory[dummy], OD_NAME_ONLY);
			(void)curse_weapon(FALSE, dummy);
			reward = format(_("%sが破壊された。", "destroying %s"), o_name);
			break;

		case REW_CURSE_AR:

			if (!inventory[INVEN_BODY].k_idx) break;
			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「汝、防具に頼ることなかれ。」", "'Thou reliest too much on thine equipment.'"));

			object_desc(o_name, &inventory[INVEN_BODY], OD_NAME_ONLY);
			(void)curse_armor();
			reward = format(_("%sが破壊された。", "destroying %s"), o_name);
			break;
		case REW_PISS_OFF:

			msg_format(_("%sの声がささやいた:", "The voice of %s whispers:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「我を怒りしめた罪を償うべし。」", "'Now thou shalt pay for annoying me.'"));

			switch (randint1(4))
			{
			case 1:
				(void)activate_ty_curse(FALSE, &count);
				reward = _("禍々しい呪いをかけられた。", "cursing");
				break;
			case 2:
				activate_hi_summon(p_ptr->y, p_ptr->x, FALSE);
				reward = _("モンスターを召喚された。", "summoning hostile monsters");
				break;
			case 3:
				if (one_in_(2))
				{
					if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) break;
					dummy = INVEN_RARM;
					if (buki_motteruka(INVEN_LARM))
					{
						dummy = INVEN_LARM;
						if (buki_motteruka(INVEN_RARM) && one_in_(2)) dummy = INVEN_RARM;
					}
					object_desc(o_name, &inventory[dummy], OD_NAME_ONLY);
					(void)curse_weapon(FALSE, dummy);
					reward = format(_("%sが破壊された。", "destroying %s"), o_name);
				}
				else
				{
					if (!inventory[INVEN_BODY].k_idx) break;
					object_desc(o_name, &inventory[INVEN_BODY], OD_NAME_ONLY);
					(void)curse_armor();
					reward = format(_("%sが破壊された。", "destroying %s"), o_name);
				}
				break;
			default:
				for (dummy = 0; dummy < A_MAX; dummy++)
				{
					(void)dec_stat(dummy, 10 + randint1(15), TRUE);
				}
				reward = _("全能力値が下がった。", "decreasing all stats");
				break;
			}
			break;

		case REW_WRATH:

			msg_format(_("%sの声が轟き渡った:", "The voice of %s thunders:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「死ぬがよい、下僕よ！」", "'Die, mortal!'"));

			take_hit(DAMAGE_LOSELIFE, p_ptr->lev * 4, wrath_reason, -1);
			for (dummy = 0; dummy < A_MAX; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint1(15), FALSE);
			}
			activate_hi_summon(p_ptr->y, p_ptr->x, FALSE);
			(void)activate_ty_curse(FALSE, &count);
			if (one_in_(2))
			{
				dummy = 0;

				if (buki_motteruka(INVEN_RARM))
				{
					dummy = INVEN_RARM;
					if (buki_motteruka(INVEN_LARM) && one_in_(2)) dummy = INVEN_LARM;
				}
				else if (buki_motteruka(INVEN_LARM)) dummy = INVEN_LARM;

				if (dummy) (void)curse_weapon(FALSE, dummy);
			}
			if (one_in_(2)) (void)curse_armor();
			break;

		case REW_DESTRUCT:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「死と破壊こそ我が喜びなり！」", "'Death and destruction! This pleaseth me!'"));

			(void)destroy_area(p_ptr->y, p_ptr->x, 25, FALSE);
			reward = _("ダンジョンが*破壊*された。", "*destruct*ing dungeon");
			break;

		case REW_GENOCIDE:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「我、汝の敵を抹殺せん！」", "'Let me relieve thee of thine oppressors!'"));
			(void)symbol_genocide(0, FALSE);
			reward = _("モンスターが抹殺された。", "genociding monsters");
			break;

		case REW_MASS_GEN:

			msg_format(_("%sの声が響き渡った:", "The voice of %s booms out:"), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("「我、汝の敵を抹殺せん！」", "'Let me relieve thee of thine oppressors!'"));

			(void)mass_genocide(0, FALSE);
			reward = _("モンスターが抹殺された。", "genociding nearby monsters");
			break;

		case REW_DISPEL_C:

			msg_format(_("%sの力が敵を攻撃するのを感じた！", "You can feel the power of %s assault your enemies!"), chaos_patrons[p_ptr->chaos_patron]);
			(void)dispel_monsters(p_ptr->lev * 4);
			break;

		case REW_IGNORE:

			msg_format(_("%sはあなたを無視した。", "%s ignores you."), chaos_patrons[p_ptr->chaos_patron]);
			break;

		case REW_SER_DEMO:

			msg_format(_("%sは褒美として悪魔の使いをよこした！", "%s rewards you with a demonic servant!"), chaos_patrons[p_ptr->chaos_patron]);

			if (!summon_specific(-1, p_ptr->y, p_ptr->x, dun_level, SUMMON_DEMON, PM_FORCE_PET, '\0'))
				msg_print(_("何も現れなかった...", "Nobody ever turns up..."));
			else
				reward = _("悪魔がペットになった。", "a demonic servant");

			break;

		case REW_SER_MONS:
			msg_format(_("%sは褒美として使いをよこした！", "%s rewards you with a servant!"), chaos_patrons[p_ptr->chaos_patron]);

			if (!summon_specific(-1, p_ptr->y, p_ptr->x, dun_level, 0, PM_FORCE_PET, '\0'))
				msg_print(_("何も現れなかった...", "Nobody ever turns up..."));
			else
				reward = _("モンスターがペットになった。", "a servant");

			break;

		case REW_SER_UNDE:
			msg_format(_("%sは褒美としてアンデッドの使いをよこした。", "%s rewards you with an undead servant!"), chaos_patrons[p_ptr->chaos_patron]);

			if (!summon_specific(-1, p_ptr->y, p_ptr->x, dun_level, SUMMON_UNDEAD, PM_FORCE_PET, '\0'))
				msg_print(_("何も現れなかった...", "Nobody ever turns up..."));
			else
				reward = _("アンデッドがペットになった。", "an undead servant");

			break;

		default:
			msg_format(_("%sの声がどもった:", "The voice of %s stammers:"),

				chaos_patrons[p_ptr->chaos_patron]);
			msg_format(_("「あー、あー、答えは %d/%d。質問は何？」", "'Uh... uh... the answer's %d/%d, what's the question?'"), type, effect);

		}
	}
	if (reward)
	{
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format(_("パトロンの報酬で%s", "The patron rewards you with %s."), reward));
	}
}

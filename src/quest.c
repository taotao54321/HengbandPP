﻿#include "angband.h"
#include "floor-events.h"
#include "quest.h"


/*!
 * @brief クエストを達成状態にする /
 * @param quest_num 達成状態にしたいクエストのID
 * @return なし
 */
void complete_quest(QUEST_IDX quest_num)
{
	quest_type* const q_ptr = &quest[quest_num];

	switch (q_ptr->type)
	{
	case QUEST_TYPE_RANDOM:
		if (record_rand_quest) do_cmd_write_nikki(NIKKI_RAND_QUEST_C, quest_num, NULL);
		break;
	default:
		if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, quest_num, NULL);
		break;
	}

	q_ptr->status = QUEST_STATUS_COMPLETED;
	q_ptr->complev = p_ptr->lev;
	update_playtime();
	q_ptr->comptime = playtime;

	if (!(q_ptr->flags & QUEST_FLAG_SILENT))
	{
		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_QUEST_CLEAR);
		msg_print(_("クエストを達成した！", "You just completed your quest!"));
		msg_print(NULL);
	}
}



/*!
 * @brief 特定の敵を倒した際にクエスト達成処理 /
 * Check for "Quest" completion when a quest monster is killed or charmed.
 * @param m_ptr 撃破したモンスターの構造体参照ポインタ
 * @return なし
 */
void check_quest_completion(monster_type *m_ptr)
{
	POSITION y, x;
	QUEST_IDX quest_num;

	bool create_stairs = FALSE;
	bool reward = FALSE;

	object_type forge;
	object_type *o_ptr;

	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Inside a quest */
	quest_num = p_ptr->inside_quest;

	/* Search for an active quest on this dungeon level */
	if (!quest_num)
	{
		QUEST_IDX i;

		for (i = max_q_idx - 1; i > 0; i--)
		{
			quest_type* const q_ptr = &quest[i];

			/* Quest is not active */
			if (q_ptr->status != QUEST_STATUS_TAKEN)
				continue;

			/* Quest is not a dungeon quest */
			if (q_ptr->flags & QUEST_FLAG_PRESET)
				continue;

			/* Quest is not on this level */
			if ((q_ptr->level != dun_level) &&
				(q_ptr->type != QUEST_TYPE_KILL_ANY_LEVEL))
				continue;

			/* Not a "kill monster" quest */
			if ((q_ptr->type == QUEST_TYPE_FIND_ARTIFACT) ||
				(q_ptr->type == QUEST_TYPE_FIND_EXIT))
				continue;

			/* Interesting quest */
			if ((q_ptr->type == QUEST_TYPE_KILL_NUMBER) ||
				(q_ptr->type == QUEST_TYPE_TOWER) ||
				(q_ptr->type == QUEST_TYPE_KILL_ALL))
				break;

			/* Interesting quest */
			if (((q_ptr->type == QUEST_TYPE_KILL_LEVEL) ||
				(q_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL) ||
				(q_ptr->type == QUEST_TYPE_RANDOM)) &&
				(q_ptr->r_idx == m_ptr->r_idx))
				break;
		}

		quest_num = i;
	}

	/* Handle the current quest */
	if (quest_num && (quest[quest_num].status == QUEST_STATUS_TAKEN))
	{
		/* Current quest */
		quest_type* const q_ptr = &quest[quest_num];

		switch (q_ptr->type)
		{
		case QUEST_TYPE_KILL_NUMBER:
		{
			q_ptr->cur_num++;

			if (q_ptr->cur_num >= q_ptr->num_mon)
			{
				complete_quest(quest_num);

				q_ptr->cur_num = 0;
			}
			break;
		}
		case QUEST_TYPE_KILL_ALL:
		{
			if (!is_hostile(m_ptr)) break;

			if (count_all_hostile_monsters() == 1)
			{
				if (q_ptr->flags & QUEST_FLAG_SILENT)
				{
					q_ptr->status = QUEST_STATUS_FINISHED;
				}
				else
				{
					complete_quest(quest_num);
				}
			}
			break;
		}
		case QUEST_TYPE_KILL_LEVEL:
		case QUEST_TYPE_RANDOM:
		{
			/* Only count valid monsters */
			if (q_ptr->r_idx != m_ptr->r_idx)
				break;

			q_ptr->cur_num++;

			if (q_ptr->cur_num >= q_ptr->max_num)
			{
				complete_quest(quest_num);

				if (!(q_ptr->flags & QUEST_FLAG_PRESET))
				{
					create_stairs = TRUE;
					p_ptr->inside_quest = 0;
				}

				/* Finish the two main quests without rewarding */
				if ((quest_num == QUEST_OBERON) || (quest_num == QUEST_SERPENT))
				{
					q_ptr->status = QUEST_STATUS_FINISHED;
				}

				if (q_ptr->type == QUEST_TYPE_RANDOM)
				{
					reward = TRUE;
					q_ptr->status = QUEST_STATUS_FINISHED;
				}
			}
			break;
		}
		case QUEST_TYPE_KILL_ANY_LEVEL:
		{
			q_ptr->cur_num++;
			if (q_ptr->cur_num >= q_ptr->max_num)
			{
				complete_quest(quest_num);
				q_ptr->cur_num = 0;
			}
			break;
		}
		case QUEST_TYPE_TOWER:
		{
			if (!is_hostile(m_ptr)) break;

			if (count_all_hostile_monsters() == 1)
			{
				q_ptr->status = QUEST_STATUS_STAGE_COMPLETED;

				if ((quest[QUEST_TOWER1].status == QUEST_STATUS_STAGE_COMPLETED) &&
					(quest[QUEST_TOWER2].status == QUEST_STATUS_STAGE_COMPLETED) &&
					(quest[QUEST_TOWER3].status == QUEST_STATUS_STAGE_COMPLETED))
				{

					complete_quest(QUEST_TOWER1);
				}
			}
			break;
		}
		}
	}

	/* Create a magical staircase */
	if (create_stairs)
	{
		POSITION ny, nx;

		/* Stagger around */
		while (cave_perma_bold(y, x) || cave[y][x].o_idx || (cave[y][x].info & CAVE_OBJECT))
		{
			/* Pick a location */
			scatter(&ny, &nx, y, x, 1, 0);

			/* Stagger */
			y = ny; x = nx;
		}

		/* Explain the staircase */
		msg_print(_("魔法の階段が現れた...", "A magical staircase appears..."));

		/* Create stairs down */
		cave_set_feat(y, x, feat_down_stair);

		/* Remember to update everything */
		p_ptr->update |= (PU_FLOW);
	}

	/*
	 * Drop quest reward
	 */
	if (reward)
	{
		int i;

		for (i = 0; i < (dun_level / 15) + 1; i++)
		{
			o_ptr = &forge;
			object_wipe(o_ptr);

			/* Make a great object */
			make_object(o_ptr, AM_GOOD | AM_GREAT);
			(void)drop_near(o_ptr, -1, y, x);
		}
	}
}

/*!
 * @brief 特定のアーティファクトを入手した際のクエスト達成処理 /
 * Check for "Quest" completion when a quest monster is killed or charmed.
 * @param o_ptr 入手したオブジェクトの構造体参照ポインタ
 * @return なし
 */
void check_find_art_quest_completion(object_type *o_ptr)
{
	QUEST_IDX i;
	/* Check if completed a quest */
	for (i = 0; i < max_q_idx; i++)
	{
		if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) &&
			(quest[i].status == QUEST_STATUS_TAKEN) &&
			(quest[i].k_idx == o_ptr->name1))
		{
			complete_quest(i);
		}
	}
}

#include "angband.h"
#include "avatar.h"
#include "spells-status.h"
#include "projection.h"

/*!
 * @brief �����X�^�[�񕜏���
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @param dam �З�
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool heal_monster(DIRECTION dir, HIT_POINT dam)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_HEAL, dir, dam, flg));
}

/*!
 * @brief �����X�^�[��������
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @param power ����
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool speed_monster(DIRECTION dir, int power)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SPEED, dir, power, flg));
}

/*!
 * @brief �����X�^�[��������
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @param power ����
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool slow_monster(DIRECTION dir, int power)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SLOW, dir, power, flg));
}

/*!
 * @brief �����X�^�[�Ö�����
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @param power ����
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool sleep_monster(DIRECTION dir, int power)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SLEEP, dir, power, flg));
}

/*!
 * @brief �����X�^�[�S��(STASIS)����
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 * @details �З͂̓v���C���[���x��*2�ɌŒ�
 */
bool stasis_monster(DIRECTION dir)
{
	return (fire_ball_hide(GF_STASIS, dir, p_ptr->lev * 2, 0));
}

/*!
 * @brief �׈��ȃ����X�^�[�S��(STASIS)����
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 * @details �З͂̓v���C���[���x��*2�ɌŒ�
 */
bool stasis_evil(DIRECTION dir)
{
	return (fire_ball_hide(GF_STASIS_EVIL, dir, p_ptr->lev * 2, 0));
}

/*!
 * @brief �����X�^�[��������
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @param plev �v���C���[���x��(=����)
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool confuse_monster(DIRECTION dir, PLAYER_LEVEL plev)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_CONF, dir, plev, flg));
}

/*!
 * @brief �����X�^�[�N�O����
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @param plev �v���C���[���x��(=����)
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool stun_monster(DIRECTION dir, PLAYER_LEVEL plev)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_STUN, dir, plev, flg));
}

/*!
 * @brief �`�F���W�����X�^�[����
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @param power ����
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool poly_monster(DIRECTION dir, int power)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	bool tester = (project_hook(GF_OLD_POLY, dir, power, flg));
	if (tester)
		chg_virtue(V_CHANCE, 1);
	return(tester);
}

/*!
 * @brief �N���[�������X�^�[����
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool clone_monster(DIRECTION dir)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_CLONE, dir, 0, flg));
}

/*!
 * @brief �����X�^�[���Q����
 * @param dir ����(5�Ȃ�΃O���[�o���ϐ� target_col/target_row �̍��W��ڕW�ɂ���)
 * @param plev �v���C���[���x��(=����)
 * @return ��p�����ۂɂ������ꍇTRUE��Ԃ�
 */
bool fear_monster(DIRECTION dir, PLAYER_LEVEL plev)
{
	BIT_FLAGS flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_TURN_ALL, dir, plev, flg));
}
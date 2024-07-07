#include "VitesseConstants.h"

using namespace AbyssEngine;

const VitesseConstants::Thruster VitesseConstants::Thrusters[static_cast<int>(Thruster::Location::Installed_Units)] =
{
	//�\�P�b�g��						//���W				//��]				//�X�P�[��
	{"rig_robo_J_backpack_end_L",	{0.0f,0.05f,0.0f},	{0.0f,0.0f,-68.0f},	{0.3f,-0.3f,-0.3f}},//BackPack_R_U,//�o�b�N�p�b�N�E��
	{"rig_robo_J_backpack_end_L",	{0.0f,0.1f,0.1f},	{0.0f,0.0f,-93.0f},	{0.3f,-0.3f,-0.3f}},//BackPack_R_D,//�o�b�N�p�b�N�E��
	{"rig_robo_J_backpack_end_R",	{0.0f,-0.05f,0.0f},	{0.0f,0.0f,68.0f},	{0.3f,0.3f,0.3f}},//BackPack_L_U,//�o�b�N�p�b�N�E��
	{"rig_robo_J_backpack_end_R",	{0.0f,-0.1f,-0.1f},	{0.0f,0.0f,93.0f},	{0.3f,0.3f,0.3f}},//BackPack_L_D,//�o�b�N�p�b�N�E��
	{"rig_J_upleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_R_U,//�E�G��������
	{"rig_J_upleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_L_U,//���G��������
	{"rig_J_lowleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_R_M,//�E�ӂ���͂�����
	{"rig_J_lowleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_R_D,//�E�ӂ���͂�����
	{"rig_J_lowleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_L_M,//���ӂ���͂�����
	{"rig_J_lowleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_L_D,//���ӂ���͂�����
	{"rig_J_upleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Waist_R,//���E��
	{"rig_J_upleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Waist_L,//������
	{"rig_J_lowleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Knee_R_R,//�E�G�A�E��
	{"rig_J_lowleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Knee_R_L,//�E�G�A����
	{"rig_J_lowleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Knee_L_R,//���G�A�E��
	{"rig_J_lowleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Knee_L_L,//���G�A����
	{"rig_sub02J_shoulder_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_R_1,//�E��1�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
	{"rig_sub02J_shoulder_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_R_2,//�E��2�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
	{"rig_sub02J_shoulder_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_R_3,//�E��3�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
	{"rig_sub02J_shoulder_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_L_1,//����1�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
	{"rig_sub02J_shoulder_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_L_2,//����2�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
	{"rig_sub02J_shoulder_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_L_3,//����3�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
	{"rig_J_upbody01",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Chest_R,//�����E��
	{"rig_J_upbody01",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Chest_L,//��������
};

























#pragma once
#include "MathHelper.h"
#include <string>

class VitesseConstants
{
public:
	struct Thruster
	{
		std::string socketName_;

		//AbyssEngine::Vector3 offsetPos_;
		//AbyssEngine::Vector3 offsetRot_;
		//AbyssEngine::Vector3 offsetScale_;

		//�X���X�^�[�e����
		enum class Location
		{
			BackPack_R_U,//�o�b�N�p�b�N�E��
			BackPack_R_D,//�o�b�N�p�b�N�E��
			BackPack_L_U,//�o�b�N�p�b�N�E��
			BackPack_L_D,//�o�b�N�p�b�N�E��

			BackLeg_R_U,//�E�G��������
			BackLeg_L_U,//���G��������

			BackLeg_R_M,//�E�ӂ���͂�����
			BackLeg_R_D,//�E�ӂ���͂�����
			BackLeg_L_M,//���ӂ���͂�����
			BackLeg_L_D,//���ӂ���͂�����

			Waist_R,//���E��
			Waist_L,//������

			Knee_R_R,//�E�G�A�E��
			Knee_R_L,//�E�G�A����
			Knee_L_R,//���G�A�E��
			Knee_L_L,//���G�A����

			Shoulder_R_1,//�E��1�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
			//Shoulder_R_2,//�E��2�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
			//Shoulder_R_3,//�E��3�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
			Shoulder_L_1,//����1�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
			//Shoulder_L_2,//����2�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��
			//Shoulder_L_3,//����3�Ԉ�ԏ�̃o�[�j�A���玞�v����1,2,3��

			Chest_R,//�����E��
			Chest_L,//��������

			Installed_Units,//���ڊ
		};
	};
	static const Thruster Thrusters[static_cast<int>(Thruster::Location::Installed_Units)];
};

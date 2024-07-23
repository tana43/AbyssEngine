#include "FadeSystem.h"
#include "Engine.h"
#include <cmath>

using namespace AbyssEngine;

FadeSystem::FadeSystem(float* data)
{
	params_.emplace_back(data);
	originParams_.emplace_back(*data);
}

FadeSystem::FadeSystem(float* datas, size_t arraySize)
{
	for (size_t i = 0; i < arraySize; i++)
	{
		params_.emplace_back(&datas[i]);
		originParams_.emplace_back(datas[i]);
	}
}

bool FadeSystem::Fade(const float& time, const float& nextParam)
{
	std::vector<float> nextParams;
	nextParams.emplace_back(nextParam);
	return Fade(time,nextParams);
}

bool FadeSystem::Fade(const float& time, const std::vector<float>& nextParams)
{
	//�P�ڂ̗v�f�������l�Ȃ�J�ڎd�؂��Ă���
	if (*params_.at(0) == nextParams.at(0))
	{
		transitioning_ = false;
	}
	else
	{
		transitioning_ = true;
	}

	//�J�ڒ��ł͂Ȃ��Ȃ�A�^�C�}�[��������
	if (!transitioning_)
	{
		time_ = time;
		timer_ = 0;

		//���f�[�^�X�V
		for (int i = 0; i < params_.size(); ++i)
		{
			originParams_[i] = *params_[i];
		}

		return true;
	}

	//�^�C�}�[�X�V
	timer_ += Time::deltaTime_;

	//�^�C�}�[���J�ڂɊ|���鎞�Ԃ��z���Ă��Ȃ���
	if (timer_ > time)
	{
		transitioning_ = false;

		//���f�[�^�ɑJ�ڐ�̐��l��������
		for (int i = 0; i < params_.size(); ++i)
		{
			*params_[i] = nextParams[i];
		}

		return true;
	}

	//�J�ڊJ�n
	//�Ƃ肠�������`���

	//�⊮��
	const float ratio = timer_ / time_;

	//�e�v�f��⊮����
	for (int i = 0; i < params_.size(); ++i)
	{
		*params_[i] = std::_Linear_for_lerp<float>(originParams_[i], nextParams[i], ratio);
	}

	return false;
}


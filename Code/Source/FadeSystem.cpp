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
	//１つ目の要素が同じ値なら遷移仕切っている
	if (*params_.at(0) == nextParams.at(0))
	{
		transitioning_ = false;
	}
	else
	{
		transitioning_ = true;
	}

	//遷移中ではないなら、タイマーを初期化
	if (!transitioning_)
	{
		time_ = time;
		timer_ = 0;

		//元データ更新
		for (int i = 0; i < params_.size(); ++i)
		{
			originParams_[i] = *params_[i];
		}

		return true;
	}

	//タイマー更新
	timer_ += Time::deltaTime_;

	//タイマーが遷移に掛かる時間を越えていないか
	if (timer_ > time)
	{
		transitioning_ = false;

		//元データに遷移先の数値を代入する
		for (int i = 0; i < params_.size(); ++i)
		{
			*params_[i] = nextParams[i];
		}

		return true;
	}

	//遷移開始
	//とりあえず線形補間

	//補完率
	const float ratio = timer_ / time_;

	//各要素を補完する
	for (int i = 0; i < params_.size(); ++i)
	{
		*params_[i] = std::_Linear_for_lerp<float>(originParams_[i], nextParams[i], ratio);
	}

	return false;
}


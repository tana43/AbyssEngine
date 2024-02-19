#include "Renderer.h"

using namespace AbyssEngine;

void Renderer::SetEnable(bool value)
{
	if (value != enabledOld_)
	{
		enabled_ = value;
		enabledOld_ = value;

		//各レンダラーごとの処理を行う
		SetActive(value);
	}
}

#include "Renderer.h"

using namespace AbyssEngine;

void Renderer::SetEnable(bool value)
{
	if (value != enabledOld_)
	{
		enabled_ = value;
		enabledOld_ = value;

		//�e�����_���[���Ƃ̏������s��
		SetActive(value);
	}
}

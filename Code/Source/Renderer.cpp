#include "Renderer.h"

using namespace AbyssEngine;

void Renderer::SetEnable(bool value)
{
	if (value)
	{
		enabled_ = value;

		//�e�����_���[���Ƃ̏������s��
		SetActive(value);
	}
}

#include "Renderer.h"

using namespace AbyssEngine;

void Renderer::SetEnable(bool value)
{
	enabled_ = value;

	//各レンダラーごとの処理を行う
	SetActive(value);
}

#include <tchar.h>
#include <locale>
#include "Texture.h"
#include "DXSystem.h"
#include "Engine.h"

using namespace AbyssEngine;

std::shared_ptr<Texture> Texture::Load(const std::string& texturePath, const u_int& textureFlg)
{
    HRESULT hr = S_OK;

    setlocale(LC_ALL, "japanese");
    wchar_t fileName[MAX_PATH] = { 0 };
    size_t ret = 0;
    mbstowcs_s(&ret, fileName, MAX_PATH, texturePath.c_str(), _TRUNCATE);

    if (const auto it = Engine::)
    {

    }

    return std::shared_ptr<Texture>();
}

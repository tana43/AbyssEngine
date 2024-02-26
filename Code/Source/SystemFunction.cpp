#include "SystemFunction.h"
#include <clocale>
#include <Windows.h>

using namespace std;

std::string SystemFunction::CreateID()
{
    //UUID(��΂ɔ��Ȃ��ŗL�̕�����)���쐬
    UUID uuid = {};
    string guid;

    (void)UuidCreate(&uuid);

    RPC_CSTR szUuid = nullptr;
    if (UuidToStringA(&uuid,&szUuid) == RPC_S_OK)
    {
        guid = reinterpret_cast<char*>(szUuid);
        RpcStringFreeA(&szUuid);
    }

    return guid;
}

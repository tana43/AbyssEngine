#include "SystemFunction.h"
#include <clocale>
#include <Windows.h>

using namespace std;

std::string SystemFunction::CreateID()
{
    //UUID(絶対に被らない固有の文字列)を作成
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

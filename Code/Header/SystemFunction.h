#pragma once
#include <string>

//デバッグ時のみ定数もImGuiでの変更が加えられるようにしておく
#if _DEBUG
typedef float CONSTANT_FLOAT;
typedef int CONSTANT_INT;
#else
typedef const float CONSTANT_FLOAT;
typedef const int CONSTANT_INT;
#endif // DEBUG


class SystemFunction
{
public:
    static std::string CreateID();//固有なUUIDを生成する
};


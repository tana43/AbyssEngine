#pragma once
#include <string>

//�f�o�b�O���̂ݒ萔��ImGui�ł̕ύX����������悤�ɂ��Ă���
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
    static std::string CreateID();//�ŗL��UUID�𐶐�����
};


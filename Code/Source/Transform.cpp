#include "Transform.h"

using namespace AbyssEngine;

Matrix Transform::CalcWorldMatrix()
{
    const auto S = Matrix::CreateScale(scale_);
    const auto R = Matrix::CreateFromYawPitchRoll(rotation_.x, rotation_.y, rotation_.z);
    const auto T = Matrix::CreateTranslation(position_);

    return S * R * T;
}

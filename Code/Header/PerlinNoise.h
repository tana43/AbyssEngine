#pragma once
#include <cstdint>
#include <array>

class PerlinNoise
{
    using Pint = std::uint_fast8_t;

public:
    constexpr PerlinNoise() = default;
    explicit PerlinNoise(const std::uint_fast32_t seed_);

    void SetSeed(const std::uint_fast32_t seed_);

    //�I�N�^�[�u�����m�C�Y���擾����
    template <typename Args>
    float Noise(const Args args) const noexcept;

    //�I�N�^�[�u�L��m�C�Y���擾���� �I�N�^�[�u�l��1~10�̒l�Őݒ�
    template <typename Args>
    float OctaveNoise(const std::size_t octaves_, const Args args_) const noexcept;


private:
    //�p�[�����m�C�Y�����֐��Q
    constexpr double GetFade(const double t_) const noexcept {
        return t_ * t_ * t_ * (t_ * (t_ * 6 - 15) + 10);
    }
    constexpr double GetLerp(const double t_, const double a_, const double b_) const noexcept {
        return a_ + t_ * (b_ - a_);
    }
    constexpr double MakeGrad(const Pint hash_, const double u_, const double v_) const noexcept {
        return (((hash_ & 1) == 0) ? u_ : -u_) + (((hash_ & 2) == 0) ? v_ : -v_);
    }
    constexpr double MakeGrad(const Pint hash_, const double x_, const double y_, const double z_) const noexcept {
        return this->MakeGrad(hash_, hash_ < 8 ? x_ : y_, hash_ < 4 ? y_ : hash_ == 12 || hash_ == 14 ? x_ : z_);
    }
    constexpr double GetGrad(const Pint hash_, const double x_, const double y_, const double z_) const noexcept {
        return this->MakeGrad(hash_ & 15, x_, y_, z_);
    }

    //�m�C�Y����
    double SetNoise(double x = 0.0, double y = 0.0, double z = 0.0)const noexcept;

    //�I�N�^�[�u�t���̃m�C�Y����
    double SetOctaveNoise(const std::size_t octaves_, double x_) const noexcept;
    double SetOctaveNoise(const std::size_t octaves_, double x_, double y_) const noexcept;
    double SetOctaveNoise(const std::size_t octaves_, double x_, double y_, double z_) const noexcept;


private:
    //�����o�ϐ�
    std::array<Pint, 512> p{ {} };

};

template<typename Args>
inline float PerlinNoise::Noise(const Args args) const noexcept
{
    return static_cast<float>(this->SetNoise(args) * 0.5 + 0.5);
}

template<typename Args>
inline float PerlinNoise::OctaveNoise(const std::size_t octaves_, const Args args_) const noexcept
{
    return static_cast<float>(this->SetOctaveNoise(octaves_, args_) * 0.5 + 0.5);
}

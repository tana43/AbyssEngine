#include "PerlinNoise.h"
#include <algorithm>
#include <random>

PerlinNoise::PerlinNoise(const std::uint_fast32_t seed)
{
	this->SetSeed(seed);
}

void PerlinNoise::SetSeed(const std::uint_fast32_t seed)
{
	for (std::size_t i = 0; i < 256; i++)
	{
		this->p[i] = static_cast<Pint>(i);
		std::shuffle(this->p.begin(), this->p.begin() + 256, std::default_random_engine(seed));
	}
}

double PerlinNoise::SetNoise(double x, double y, double z)const noexcept
{
    const std::size_t x_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(x)) & 255) };
    const std::size_t y_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(y)) & 255) };
    const std::size_t z_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(z)) & 255) };
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);
    const double u{ this->GetFade(x) };
    const double v{ this->GetFade(y) };
    const double w{ this->GetFade(z) };
    const std::size_t a0{ static_cast<std::size_t>(this->p[x_int] + y_int) };
    const std::size_t a1{ static_cast<std::size_t>(this->p[a0] + z_int) };
    const std::size_t a2{ static_cast<std::size_t>(this->p[a0 + 1] + z_int) };
    const std::size_t b0{ static_cast<std::size_t>(this->p[x_int + 1] + y_int) };
    const std::size_t b1{ static_cast<std::size_t>(this->p[b0] + z_int) };
    const std::size_t b2{ static_cast<std::size_t>(this->p[b0 + 1] + z_int) };

    return this->GetLerp(w,
        this->GetLerp(v,
            this->GetLerp(u, this->GetGrad(this->p[a1], x, y, z), this->GetGrad(this->p[b1], x - 1, y, z)),
            this->GetLerp(u, this->GetGrad(this->p[a2], x, y - 1, z), this->GetGrad(this->p[b2], x - 1, y - 1, z))),
        this->GetLerp(v,
            this->GetLerp(u, this->GetGrad(this->p[a1 + 1], x, y, z - 1), this->GetGrad(this->p[b1 + 1], x - 1, y, z - 1)),
            this->GetLerp(u, this->GetGrad(this->p[a2 + 1], x, y - 1, z - 1), this->GetGrad(this->p[b2 + 1], x - 1, y - 1, z - 1))));
}

double PerlinNoise::SetOctaveNoise(const std::size_t octaves_, double x_) const noexcept
{
    double noise_value{};
    double amp{ 1.0 };
    for (std::size_t i{}; i < octaves_; ++i) {
        noise_value += this->SetNoise(x_) * amp;
        x_ *= 2.0;
        amp *= 0.5;
    }
    return noise_value;
}

double PerlinNoise::SetOctaveNoise(const std::size_t octaves_, double x_, double y_) const noexcept
{
    double noise_value{};
    double amp{ 1.0 };
    for (std::size_t i{}; i < octaves_; ++i) {
        noise_value += this->SetNoise(x_, y_) * amp;
        x_ *= 2.0;
        y_ *= 2.0;
        amp *= 0.5;
    }
    return noise_value;
}

double PerlinNoise::SetOctaveNoise(const std::size_t octaves_, double x_, double y_, double z_) const noexcept
{
    double noise_value{};
    double amp{ 1.0 };
    for (std::size_t i{}; i < octaves_; ++i) {
        noise_value += this->SetNoise(x_, y_, z_) * amp;
        x_ *= 2.0;
        y_ *= 2.0;
        z_ *= 2.0;
        amp *= 0.5;
    }
    return noise_value;
}

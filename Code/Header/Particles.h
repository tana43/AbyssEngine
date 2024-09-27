#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "MathHelper.h"

#include <vector>

#define NUMTHREADS_X 16

namespace AbyssEngine
{
	class Particle
	{
	public:
		int state_ = 0;

		Vector4 color_ = { 1, 1, 1, 1 };

		Vector3 position_ = { 0, 0, 0 };
		float mass_ = 1.0f;

		float angle_ = 0.0f;
		float angularSpeed_ = 0.0f;
		Vector3 velocity_ = { 0, 0, 0 };

		float flashLifespan_ = 1.0f;
		float age_ = 0.0f;

		Vector2 size_ = { 0,0 }; // x: spawn, y: despawn

		int chip_ = 0;
	};


	class ParticleSystem
	{
	public:
		ParticleSystem(int particleCount);
		ParticleSystem(const ParticleSystem&) = delete;
		ParticleSystem& operator=(const ParticleSystem&) = delete;
		ParticleSystem(ParticleSystem&&) noexcept = delete;
		ParticleSystem& operator=(ParticleSystem&&) noexcept = delete;
		virtual ~ParticleSystem() = default;

		//çXêVèàóù
		void Integrate();

		//èâä˙âª
		void Initialize();

		//ï`âÊ
		void Render();

	public:
		const int Max_Particle_Count;

		struct ParticleSystemConstants
		{
			Vector4 emissionColor_ = { 1, 1, 1, 1 };
			Vector4 emissionPosition_ = {};
			Vector2 emissionSize_ = { 0.5f, 0.0f }; // x: spawn, y: despawn
			Vector2 emissionConeAngle_ = { 0.0f, 0.2f }; // x: minimum radian, y: maximum radian
			Vector2 emissionSpeed_ = { 0.5f , 1.0f }; // x: minimum speed, y: maximum speed
			Vector2 emissionAngularSpeed_ = { 0.0f, 1.0f }; // x: minimum angular speed, y: maximum angular speed
			Vector2 flashLifespan_ = { 1.0f, 2.5f }; // x: minimum second, y: maximum second
			Vector2 spawnDelay_ = { 0.0f, 5.0f }; // x: minimum second, y: maximum second
			Vector2 fadeDuration_ = { 1.0f, 2.5f }; // x: fade in, y: fade out

			float time_ = 0.0f;
			float deltaTime_ = 0.0f;
			float noiseScale_ = 0.001f;
			float gravity_ = -0.1f;

			DirectX::XMUINT2 spriteSheetGrid_ = { 1, 1 };

			int Max_Particle_Count = 0;
			float pads_[3];
		};
		ParticleSystemConstants particleSystemData_;

		Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer_;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav_;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSrv_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> particleVS_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> particlePS_;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> particleGS_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> particleCS_;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> particleInitializerCS_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;

		
	};
}

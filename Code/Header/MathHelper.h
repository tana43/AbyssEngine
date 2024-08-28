#pragma once
#include <cassert>
#include <DirectXMath.h>
#include <memory>
#ifndef XM_CONSTEXPR
#define XM_CONSTEXPR
#endif

/////////////////////////////////////////////////////
//
// DXTK����Math�N���X�Ɋ֐��̒ǉ��Ȃǂ������Ȃ�������
//
/////////////////////////////////////////////////////

namespace AbyssEngine
{
	class Math
	{
	public:
		//�����_���֐�
		static float RandomRange(float min, float max)
		{
			// 0.0�`1.0�̊Ԃ܂ł̃����_���l
			float value = static_cast<float>(rand()) / RAND_MAX;

			// min�`max�܂ł̃����_���l�ɕϊ�
			return min + (max - min) * value;
		}
	};

	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Matrix;
	struct Quaternion;

	struct Vector2 : public DirectX::XMFLOAT2
	{
		Vector2() noexcept : DirectX::XMFLOAT2(0.f, 0.f) {}
		XM_CONSTEXPR explicit Vector2(float x) : DirectX::XMFLOAT2(x, x) {}
		XM_CONSTEXPR Vector2(float _x, float _y) : DirectX::XMFLOAT2(_x, _y) {}
		explicit Vector2(_In_reads_(2) const float* pArray) : DirectX::XMFLOAT2(pArray) {}
		Vector2(DirectX::FXMVECTOR V) { XMStoreFloat2(this, V); }
		Vector2(const DirectX::XMFLOAT2& V) { this->x = V.x; this->y = V.y; }
		explicit Vector2(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; }

		Vector2(const Vector2&) = default;
		Vector2& operator=(const Vector2&) = default;

		Vector2(Vector2&&) = default;
		Vector2& operator=(Vector2&&) = default;

		operator DirectX::XMVECTOR() const { return XMLoadFloat2(this); }

		// Comparison operators
		bool operator == (const Vector2& V) const;
		bool operator != (const Vector2& V) const;

		// Assignment operators
		Vector2& operator= (const DirectX::XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; return *this; }
		Vector2& operator+= (const Vector2& V);
		Vector2& operator-= (const Vector2& V);
		Vector2& operator*= (const Vector2& V);
		Vector2& operator*= (float S);
		Vector2& operator/= (float S);

		// Unary operators
		Vector2 operator+ () const { return *this; }
		Vector2 operator- () const { return Vector2(-x, -y); }

		// Vector operations
		bool InBounds(const Vector2& Bounds) const;

		float Length() const;
		float LengthSquared() const;

		float Dot(const Vector2& V) const;
		void Cross(const Vector2& V, Vector2& result) const;
		Vector2 Cross(const Vector2& V) const;

		void Normalize();
		void Normalize(Vector2& result) const;

		void Clamp(const Vector2& vmin, const Vector2& vmax);
		void Clamp(const Vector2& vmin, const Vector2& vmax, Vector2& result) const;

		// Static functions
		static float Distance(const Vector2& v1, const Vector2& v2);
		static float DistanceSquared(const Vector2& v1, const Vector2& v2);

		static void Min(const Vector2& v1, const Vector2& v2, Vector2& result);
		static Vector2 Min(const Vector2& v1, const Vector2& v2);

		static void Max(const Vector2& v1, const Vector2& v2, Vector2& result);
		static Vector2 Max(const Vector2& v1, const Vector2& v2);

		static void Lerp(const Vector2& v1, const Vector2& v2, float t, Vector2& result);
		static Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);

		static void SmoothStep(const Vector2& v1, const Vector2& v2, float t, Vector2& result);
		static Vector2 SmoothStep(const Vector2& v1, const Vector2& v2, float t);

		static void Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g, Vector2& result);
		static Vector2 Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g);

		static void CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t, Vector2& result);
		static Vector2 CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t);

		static void Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t, Vector2& result);
		static Vector2 Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t);

		static void Reflect(const Vector2& ivec, const Vector2& nvec, Vector2& result);
		static Vector2 Reflect(const Vector2& ivec, const Vector2& nvec);

		static void Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex, Vector2& result);
		static Vector2 Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex);

		static void Transform(const Vector2& v, const Quaternion& quat, Vector2& result);
		static Vector2 Transform(const Vector2& v, const Quaternion& quat);

		static void Transform(const Vector2& v, const Matrix& m, Vector2& result);
		static Vector2 Transform(const Vector2& v, const Matrix& m);
		static void Transform(_In_reads_(Count) const Vector2* varray, size_t Count, const Matrix& m, _Out_writes_(Count) Vector2* resultArray);

		static void Transform(const Vector2& v, const Matrix& m, Vector4& result);
		static void Transform(_In_reads_(Count) const Vector2* varray, size_t Count, const Matrix& m, _Out_writes_(Count) Vector4* resultArray);

		static void TransformNormal(const Vector2& v, const Matrix& m, Vector2& result);
		static Vector2 TransformNormal(const Vector2& v, const Matrix& m);
		static void TransformNormal(_In_reads_(Count) const Vector2* varray, size_t Count, const Matrix& m, _Out_writes_(Count) Vector2* resultArray);

		// Constants
		static const Vector2 Zero;
		static const Vector2 One;
		static const Vector2 UnitX;
		static const Vector2 UnitY;

	
	};

	// Binary operators
	Vector2 operator+ (const Vector2& V1, const Vector2& V2);
	Vector2 operator- (const Vector2& V1, const Vector2& V2);
	Vector2 operator* (const Vector2& V1, const Vector2& V2);
	Vector2 operator* (const Vector2& V, float S);
	Vector2 operator/ (const Vector2& V1, const Vector2& V2);
	Vector2 operator* (float S, const Vector2& V);

	//------------------------------------------------------------------------------
	// 3D vector
	struct Vector3 : public DirectX::XMFLOAT3
	{
		Vector3() noexcept : DirectX::XMFLOAT3(0.f, 0.f, 0.f) {}
		XM_CONSTEXPR explicit Vector3(float x) : DirectX::XMFLOAT3(x, x, x) {}
		XM_CONSTEXPR Vector3(float _x, float _y, float _z) : DirectX::XMFLOAT3(_x, _y, _z) {}
		explicit Vector3(_In_reads_(3) const float* pArray) : DirectX::XMFLOAT3(pArray) {}
		Vector3(DirectX::FXMVECTOR V) { XMStoreFloat3(this, V); }
		Vector3(const DirectX::XMFLOAT3& V) { this->x = V.x; this->y = V.y; this->z = V.z; }
		explicit Vector3(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; }

		Vector3(const Vector3&) = default;
		Vector3& operator=(const Vector3&) = default;

		Vector3(Vector3&&) = default;
		Vector3& operator=(Vector3&&) = default;

		operator DirectX::XMVECTOR() const { return XMLoadFloat3(this); }

		// Comparison operators
		bool operator == (const Vector3& V) const;
		bool operator != (const Vector3& V) const;

		// Assignment operators
		Vector3& operator= (const DirectX::XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; return *this; }
		Vector3& operator+= (const Vector3& V);
		Vector3& operator-= (const Vector3& V);
		Vector3& operator*= (const Vector3& V);
		Vector3& operator*= (float S);
		Vector3& operator/= (float S);

		// Unary operators
		Vector3 operator+ () const { return *this; }
		Vector3 operator- () const;

		// Vector operations
		bool InBounds(const Vector3& Bounds) const;

		float Length() const;
		float LengthSquared() const;

		float Dot(const Vector3& V) const;
		void Cross(const Vector3& V, Vector3& result) const;
		Vector3 Cross(const Vector3& V) const;

		void Normalize();
		void Normalize(Vector3& result) const;
		Vector3 Normalize() const;

		void Clamp(const Vector3& vmin, const Vector3& vmax);
		void Clamp(const Vector3& vmin, const Vector3& vmax, Vector3& result) const;

		// Static functions
		static float Distance(const Vector3& v1, const Vector3& v2);
		static float DistanceSquared(const Vector3& v1, const Vector3& v2);

		static void Min(const Vector3& v1, const Vector3& v2, Vector3& result);
		static Vector3 Min(const Vector3& v1, const Vector3& v2);

		static void Max(const Vector3& v1, const Vector3& v2, Vector3& result);
		static Vector3 Max(const Vector3& v1, const Vector3& v2);

		static void Lerp(const Vector3& v1, const Vector3& v2, float t, Vector3& result);
		static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

		static void SmoothStep(const Vector3& v1, const Vector3& v2, float t, Vector3& result);
		static Vector3 SmoothStep(const Vector3& v1, const Vector3& v2, float t);

		static void Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g, Vector3& result);
		static Vector3 Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g);

		static void CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t, Vector3& result);
		static Vector3 CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t);

		static void Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t, Vector3& result);
		static Vector3 Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t);

		static void Reflect(const Vector3& ivec, const Vector3& nvec, Vector3& result);
		static Vector3 Reflect(const Vector3& ivec, const Vector3& nvec);

		static void Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex, Vector3& result);
		static Vector3 Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex);

		static void Transform(const Vector3& v, const Quaternion& quat, Vector3& result);
		static Vector3 Transform(const Vector3& v, const Quaternion& quat);

		static void Transform(const Vector3& v, const Matrix& m, Vector3& result);
		static Vector3 Transform(const Vector3& v, const Matrix& m);
		static void Transform(_In_reads_(Count) const Vector3* varray, size_t Count, const Matrix& m, _Out_writes_(Count) Vector3* resultArray);

		static void Transform(const Vector3& v, const Matrix& m, Vector4& result);
		static void Transform(_In_reads_(Count) const Vector3* varray, size_t Count, const Matrix& m, _Out_writes_(Count) Vector4* resultArray);

		static void TransformNormal(const Vector3& v, const Matrix& m, Vector3& result);
		static Vector3 TransformNormal(const Vector3& v, const Matrix& m);
		static void TransformNormal(_In_reads_(Count) const Vector3* varray, size_t Count, const Matrix& m, _Out_writes_(Count) Vector3* resultArray);

		static Vector3 Normalize(const Vector3& v);

		// Constants
		static const Vector3 Zero;
		static const Vector3 One;
		static const Vector3 UnitX;
		static const Vector3 UnitY;
		static const Vector3 UnitZ;
		static const Vector3 Up;
		static const Vector3 Down;
		static const Vector3 Right;
		static const Vector3 Left;
		static const Vector3 Forward;
		static const Vector3 Backward;
	};

	// Binary operators
	Vector3 operator+ (const Vector3& V1, const Vector3& V2);
	Vector3 operator- (const Vector3& V1, const Vector3& V2);
	Vector3 operator* (const Vector3& V1, const Vector3& V2);
	Vector3 operator* (const Vector3& V, float S);
	Vector3 operator/ (const Vector3& V1, const Vector3& V2);
	Vector3 operator* (float S, const Vector3& V);

	//------------------------------------------------------------------------------
	// 4D vector
	struct Vector4 : public DirectX::XMFLOAT4
	{
		Vector4() noexcept : DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f) {}
		XM_CONSTEXPR explicit Vector4(float x) : DirectX::XMFLOAT4(x, x, x, x) {}
		XM_CONSTEXPR Vector4(float _x, float _y, float _z, float _w) : DirectX::XMFLOAT4(_x, _y, _z, _w) {}
		explicit Vector4(_In_reads_(4) const float* pArray) : DirectX::XMFLOAT4(pArray) {}
		Vector4(DirectX::FXMVECTOR V) { XMStoreFloat4(this, V); }
		Vector4(const DirectX::XMFLOAT4& V) { this->x = V.x; this->y = V.y; this->z = V.z; this->w = V.w; }
		explicit Vector4(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		Vector4(const Vector4&) = default;
		Vector4& operator=(const Vector4&) = default;

		Vector4(Vector4&&) = default;
		Vector4& operator=(Vector4&&) = default;

		operator DirectX::XMVECTOR() const { return XMLoadFloat4(this); }

		// Comparison operators
		bool operator == (const Vector4& V) const;
		bool operator != (const Vector4& V) const;

		// Assignment operators
		Vector4& operator= (const DirectX::XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
		Vector4& operator+= (const Vector4& V);
		Vector4& operator-= (const Vector4& V);
		Vector4& operator*= (const Vector4& V);
		Vector4& operator*= (float S);
		Vector4& operator/= (float S);

		// Unary operators
		Vector4 operator+ () const { return *this; }
		Vector4 operator- () const;

		// Vector operations
		bool InBounds(const Vector4& Bounds) const;

		float Length() const;
		float LengthSquared() const;

		float Dot(const Vector4& V) const;
		void Cross(const Vector4& v1, const Vector4& v2, Vector4& result) const;
		Vector4 Cross(const Vector4& v1, const Vector4& v2) const;

		void Normalize();
		void Normalize(Vector4& result) const;

		void Clamp(const Vector4& vmin, const Vector4& vmax);
		void Clamp(const Vector4& vmin, const Vector4& vmax, Vector4& result) const;

		// Static functions
		static float Distance(const Vector4& v1, const Vector4& v2);
		static float DistanceSquared(const Vector4& v1, const Vector4& v2);

		static void Min(const Vector4& v1, const Vector4& v2, Vector4& result);
		static Vector4 Min(const Vector4& v1, const Vector4& v2);

		static void Max(const Vector4& v1, const Vector4& v2, Vector4& result);
		static Vector4 Max(const Vector4& v1, const Vector4& v2);

		static void Lerp(const Vector4& v1, const Vector4& v2, float t, Vector4& result);
		static Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);

		static void SmoothStep(const Vector4& v1, const Vector4& v2, float t, Vector4& result);
		static Vector4 SmoothStep(const Vector4& v1, const Vector4& v2, float t);

		static void Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g, Vector4& result);
		static Vector4 Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g);

		static void CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t, Vector4& result);
		static Vector4 CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t);

		static void Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t, Vector4& result);
		static Vector4 Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t);

		static void Reflect(const Vector4& ivec, const Vector4& nvec, Vector4& result);
		static Vector4 Reflect(const Vector4& ivec, const Vector4& nvec);

		static void Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex, Vector4& result);
		static Vector4 Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex);

		static void Transform(const Vector2& v, const Quaternion& quat, Vector4& result);
		static Vector4 Transform(const Vector2& v, const Quaternion& quat);

		static void Transform(const Vector3& v, const Quaternion& quat, Vector4& result);
		static Vector4 Transform(const Vector3& v, const Quaternion& quat);

		static void Transform(const Vector4& v, const Quaternion& quat, Vector4& result);
		static Vector4 Transform(const Vector4& v, const Quaternion& quat);

		static void Transform(const Vector4& v, const Matrix& m, Vector4& result);
		static Vector4 Transform(const Vector4& v, const Matrix& m);
		static void Transform(_In_reads_(Count) const Vector4* varray, size_t Count, const Matrix& m, _Out_writes_(Count) Vector4* resultArray);

		// Constants
		static const Vector4 Zero;
		static const Vector4 One;
		static const Vector4 UnitX;
		static const Vector4 UnitY;
		static const Vector4 UnitZ;
		static const Vector4 UnitW;
	};

	// Binary operators
	Vector4 operator+ (const Vector4& V1, const Vector4& V2);
	Vector4 operator- (const Vector4& V1, const Vector4& V2);
	Vector4 operator* (const Vector4& V1, const Vector4& V2);
	Vector4 operator* (const Vector4& V, float S);
	Vector4 operator/ (const Vector4& V1, const Vector4& V2);
	Vector4 operator* (float S, const Vector4& V);

	//------------------------------------------------------------------------------
	// 4x4 Matrix (assumes right-handed cooordinates)
	struct Matrix : public DirectX::XMFLOAT4X4
	{
		Matrix() noexcept
			: DirectX::XMFLOAT4X4(1.f, 0, 0, 0,
				0, 1.f, 0, 0,
				0, 0, 1.f, 0,
				0, 0, 0, 1.f)
		{
		}
		XM_CONSTEXPR Matrix(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
			: DirectX::XMFLOAT4X4(m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33)
		{
		}
		explicit Matrix(const Vector3& r0, const Vector3& r1, const Vector3& r2)
			: DirectX::XMFLOAT4X4(r0.x, r0.y, r0.z, 0,
				r1.x, r1.y, r1.z, 0,
				r2.x, r2.y, r2.z, 0,
				0, 0, 0, 1.f)
		{
		}
		explicit Matrix(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3)
			: DirectX::XMFLOAT4X4(r0.x, r0.y, r0.z, r0.w,
				r1.x, r1.y, r1.z, r1.w,
				r2.x, r2.y, r2.z, r2.w,
				r3.x, r3.y, r3.z, r3.w)
		{
		}
		Matrix(const DirectX::XMFLOAT4X4& M) { memcpy_s(this, sizeof(float) * 16, &M, sizeof(DirectX::XMFLOAT4X4)); }
		Matrix(const DirectX::XMFLOAT3X3& M);
		Matrix(const DirectX::XMFLOAT4X3& M);

		explicit Matrix(_In_reads_(16) const float* pArray) : DirectX::XMFLOAT4X4(pArray) {}
		Matrix(DirectX::CXMMATRIX M) { XMStoreFloat4x4(this, M); }

		Matrix(const Matrix&) = default;
		Matrix& operator=(const Matrix&) = default;

		Matrix(Matrix&&) = default;
		Matrix& operator=(Matrix&&) = default;

		operator DirectX::XMMATRIX() const { return XMLoadFloat4x4(this); }

		// Comparison operators
		bool operator == (const Matrix& M) const;
		bool operator != (const Matrix& M) const;

		// Assignment operators
		Matrix& operator= (const DirectX::XMFLOAT3X3& M);
		Matrix& operator= (const DirectX::XMFLOAT4X3& M);
		Matrix& operator+= (const Matrix& M);
		Matrix& operator-= (const Matrix& M);
		Matrix& operator*= (const Matrix& M);
		Matrix& operator*= (float S);
		Matrix& operator/= (float S);

		Matrix& operator/= (const Matrix& M);
		// Element-wise divide

	// Unary operators
		Matrix operator+ () const { return *this; }
		Matrix operator- () const;

		// Properties
		Vector3 Up() const { return Vector3(_21, _22, _23); }
		void Up(const Vector3& v) { _21 = v.x; _22 = v.y; _23 = v.z; }

		Vector3 Down() const { return Vector3(-_21, -_22, -_23); }
		void Down(const Vector3& v) { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

		Vector3 Right() const { return Vector3(_11, _12, _13); }
		void Right(const Vector3& v) { _11 = v.x; _12 = v.y; _13 = v.z; }

		Vector3 Left() const { return Vector3(-_11, -_12, -_13); }
		void Left(const Vector3& v) { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

		Vector3 Forward() const { return Vector3(-_31, -_32, -_33); }
		void Forward(const Vector3& v) { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

		Vector3 Backward() const { return Vector3(_31, _32, _33); }
		void Backward(const Vector3& v) { _31 = v.x; _32 = v.y; _33 = v.z; }

		Vector3 Translation() const { return Vector3(_41, _42, _43); }
		void Translation(const Vector3& v) { _41 = v.x; _42 = v.y; _43 = v.z; }

		// Matrix operations
		bool Decompose(Vector3& scale_, Quaternion& rotation_, Vector3& translation_);

		Matrix Transpose() const;
		void Transpose(Matrix& result) const;

		Matrix Invert() const;
		void Invert(Matrix& result) const;

		float Determinant() const;

		// Static functions
		static Matrix CreateBillboard(const Vector3& object, const Vector3& cameraPosition, const Vector3& cameraUp, _In_opt_ const Vector3* cameraForward = nullptr);

		static Matrix CreateConstrainedBillboard(const Vector3& object, const Vector3& cameraPosition, const Vector3& rotateAxis,
			_In_opt_ const Vector3* cameraForward = nullptr, _In_opt_ const Vector3* objectForward = nullptr);

		static Matrix CreateTranslation(const Vector3& position_);
		static Matrix CreateTranslation(float x, float y, float z);

		static Matrix CreateScale(const Vector3& scales);
		static Matrix CreateScale(float xs, float ys, float zs);
		static Matrix CreateScale(float scale_);

		static Matrix CreateRotationX(float radians);
		static Matrix CreateRotationY(float radians);
		static Matrix CreateRotationZ(float radians);

		static Matrix CreateFromAxisAngle(const Vector3& axis, float angle);

		static Matrix CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane);
		static Matrix CreatePerspective(float width, float height_, float nearPlane, float farPlane);
		static Matrix CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlane, float farPlane);
		static Matrix CreateOrthographic(float width, float height_, float zNearPlane, float zFarPlane);
		static Matrix CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

		static Matrix CreateLookAt(const Vector3& position_, const Vector3& target, const Vector3& up);
		static Matrix CreateWorld(const Vector3& position_, const Vector3& forward, const Vector3& up);

		static Matrix CreateFromQuaternion(const Quaternion& quat);

		static Matrix CreateFromYawPitchRoll(float yaw, float pitch, float roll);

		//static Matrix CreateShadow(const Vector3& lightDir, const Plane& plane);

		//static Matrix CreateReflection(const Plane& plane);

		static void Lerp(const Matrix& M1, const Matrix& M2, float t, Matrix& result);
		static Matrix Lerp(const Matrix& M1, const Matrix& M2, float t);

		static void Transform(const Matrix& M, const Quaternion& rotation_, Matrix& result);
		static Matrix Transform(const Matrix& M, const Quaternion& rotation_);

		// Constants
		static const Matrix Identity;
	};

	// Binary operators
	Matrix operator+ (const Matrix& M1, const Matrix& M2);
	Matrix operator- (const Matrix& M1, const Matrix& M2);
	Matrix operator* (const Matrix& M1, const Matrix& M2);
	Matrix operator* (const Matrix& M, float S);
	Matrix operator/ (const Matrix& M, float S);
	Matrix operator/ (const Matrix& M1, const Matrix& M2);
	// Element-wise divide
	Matrix operator* (float S, const Matrix& M);

	struct Quaternion : public DirectX::XMFLOAT4
	{
		Quaternion() noexcept : DirectX::XMFLOAT4(0, 0, 0, 1.f) {}
		XM_CONSTEXPR Quaternion(float _x, float _y, float _z, float _w) : DirectX::XMFLOAT4(_x, _y, _z, _w) {}
		Quaternion(const Vector3& v, float scalar) : DirectX::XMFLOAT4(v.x, v.y, v.z, scalar) {}
		explicit Quaternion(const Vector4& v) : DirectX::XMFLOAT4(v.x, v.y, v.z, v.w) {}
		explicit Quaternion(_In_reads_(4) const float* pArray) : DirectX::XMFLOAT4(pArray) {}
		Quaternion(DirectX::FXMVECTOR V) { XMStoreFloat4(this, V); }
		Quaternion(const DirectX::XMFLOAT4& q) { this->x = q.x; this->y = q.y; this->z = q.z; this->w = q.w; }
		explicit Quaternion(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		Quaternion(const Quaternion&) = default;
		Quaternion& operator=(const Quaternion&) = default;

		Quaternion(Quaternion&&) = default;
		Quaternion& operator=(Quaternion&&) = default;

		operator DirectX::XMVECTOR() const { return XMLoadFloat4(this); }

		// Comparison operators
		bool operator == (const Quaternion& q) const;
		bool operator != (const Quaternion& q) const;

		// Assignment operators
		Quaternion& operator= (const DirectX::XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3];  return *this; }
		Quaternion& operator+= (const Quaternion& q);
		Quaternion& operator-= (const Quaternion& q);
		Quaternion& operator*= (const Quaternion& q);
		Quaternion& operator*= (float S);
		Quaternion& operator/= (const Quaternion& q);

		// Unary operators
		Quaternion operator+ () const { return *this; }
		Quaternion operator- () const;

		// Quaternion operations
		float Length() const;
		float LengthSquared() const;

		void Normalize();
		void Normalize(Quaternion& result) const;

		void Conjugate();
		void Conjugate(Quaternion& result) const;

		void Inverse(Quaternion& result) const;

		float Dot(const Quaternion& Q) const;

		Vector3 To_Euler() const;
		void Set_Euler(Vector3 V);

		// Static functions
		static Quaternion AngleAxis(const Vector3& axis, float angle);
		static Quaternion Euler(float x, float y, float z);
		static Quaternion Euler(Vector3 V);
		static Quaternion CreateFromRotationMatrix(const Matrix& M);

		static void Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);

		static void Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);

		static void Concatenate(const Quaternion& q1, const Quaternion& q2, Quaternion& result);
		static Quaternion Concatenate(const Quaternion& q1, const Quaternion& q2);

		// Constants
		static const Quaternion Identity;
	};

	// Binary operators
	Quaternion operator+ (const Quaternion& Q1, const Quaternion& Q2);
	Quaternion operator- (const Quaternion& Q1, const Quaternion& Q2);
	Quaternion operator* (const Quaternion& Q1, const Quaternion& Q2);
	Quaternion operator* (const Quaternion& Q, float S);
	Quaternion operator/ (const Quaternion& Q1, const Quaternion& Q2);
	Quaternion operator* (float S, const Quaternion& Q);

	/****************************************************************************
	 *
	 * Vector2
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 // Comparision operators
	 //------------------------------------------------------------------------------

	inline bool Vector2::operator == (const Vector2& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		return XMVector2Equal(v1, v2);
	}

	inline bool Vector2::operator != (const Vector2& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		return XMVector2NotEqual(v1, v2);
	}

	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Vector2& Vector2::operator+= (const Vector2& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		XMVECTOR X = XMVectorAdd(v1, v2);
		XMStoreFloat2(this, X);
		return *this;
	}

	inline Vector2& Vector2::operator-= (const Vector2& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		XMVECTOR X = XMVectorSubtract(v1, v2);
		XMStoreFloat2(this, X);
		return *this;
	}

	inline Vector2& Vector2::operator*= (const Vector2& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		XMVECTOR X = XMVectorMultiply(v1, v2);
		XMStoreFloat2(this, X);
		return *this;
	}

	inline Vector2& Vector2::operator*= (float S)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR X = XMVectorScale(v1, S);
		XMStoreFloat2(this, X);
		return *this;
	}

	inline Vector2& Vector2::operator/= (float S)
	{
		using namespace DirectX;
		assert(S != 0.0f);
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR X = XMVectorScale(v1, 1.f / S);
		XMStoreFloat2(this, X);
		return *this;
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Vector2 operator+ (const Vector2& V1, const Vector2& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&V1);
		XMVECTOR v2 = XMLoadFloat2(&V2);
		XMVECTOR X = XMVectorAdd(v1, v2);
		Vector2 R;
		XMStoreFloat2(&R, X);
		return R;
	}

	inline Vector2 operator- (const Vector2& V1, const Vector2& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&V1);
		XMVECTOR v2 = XMLoadFloat2(&V2);
		XMVECTOR X = XMVectorSubtract(v1, v2);
		Vector2 R;
		XMStoreFloat2(&R, X);
		return R;
	}

	inline Vector2 operator* (const Vector2& V1, const Vector2& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&V1);
		XMVECTOR v2 = XMLoadFloat2(&V2);
		XMVECTOR X = XMVectorMultiply(v1, v2);
		Vector2 R;
		XMStoreFloat2(&R, X);
		return R;
	}

	inline Vector2 operator* (const Vector2& V, float S)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&V);
		XMVECTOR X = XMVectorScale(v1, S);
		Vector2 R;
		XMStoreFloat2(&R, X);
		return R;
	}

	inline Vector2 operator/ (const Vector2& V1, const Vector2& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&V1);
		XMVECTOR v2 = XMLoadFloat2(&V2);
		XMVECTOR X = XMVectorDivide(v1, v2);
		Vector2 R;
		XMStoreFloat2(&R, X);
		return R;
	}

	inline Vector2 operator* (float S, const Vector2& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&V);
		XMVECTOR X = XMVectorScale(v1, S);
		Vector2 R;
		XMStoreFloat2(&R, X);
		return R;
	}

	//------------------------------------------------------------------------------
	// Vector operations
	//------------------------------------------------------------------------------

	inline bool Vector2::InBounds(const Vector2& Bounds) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&Bounds);
		return XMVector2InBounds(v1, v2);
	}

	inline float Vector2::Length() const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR X = XMVector2Length(v1);
		return XMVectorGetX(X);
	}

	inline float Vector2::LengthSquared() const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR X = XMVector2LengthSq(v1);
		return XMVectorGetX(X);
	}

	inline float Vector2::Dot(const Vector2& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		XMVECTOR X = XMVector2Dot(v1, v2);
		return XMVectorGetX(X);
	}

	inline void Vector2::Cross(const Vector2& V, Vector2& result) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		XMVECTOR R = XMVector2Cross(v1, v2);
		XMStoreFloat2(&result, R);
	}

	inline Vector2 Vector2::Cross(const Vector2& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		XMVECTOR R = XMVector2Cross(v1, v2);

		Vector2 result;
		XMStoreFloat2(&result, R);
		return result;
	}

	inline void Vector2::Normalize()
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR X = XMVector2Normalize(v1);
		XMStoreFloat2(this, X);
	}

	inline void Vector2::Normalize(Vector2& result) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR X = XMVector2Normalize(v1);
		XMStoreFloat2(&result, X);
	}

	inline void Vector2::Clamp(const Vector2& vmin, const Vector2& vmax)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&vmin);
		XMVECTOR v3 = XMLoadFloat2(&vmax);
		XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat2(this, X);
	}

	inline void Vector2::Clamp(const Vector2& vmin, const Vector2& vmax, Vector2& result) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&vmin);
		XMVECTOR v3 = XMLoadFloat2(&vmax);
		XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat2(&result, X);
	}

	//------------------------------------------------------------------------------
	// Static functions
	//------------------------------------------------------------------------------

	inline float Vector2::Distance(const Vector2& v1, const Vector2& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR V = XMVectorSubtract(x2, x1);
		XMVECTOR X = XMVector2Length(V);
		return XMVectorGetX(X);
	}

	inline float Vector2::DistanceSquared(const Vector2& v1, const Vector2& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR V = XMVectorSubtract(x2, x1);
		XMVECTOR X = XMVector2LengthSq(V);
		return XMVectorGetX(X);
	}

	inline void Vector2::Min(const Vector2& v1, const Vector2& v2, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR X = XMVectorMin(x1, x2);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Min(const Vector2& v1, const Vector2& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR X = XMVectorMin(x1, x2);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::Max(const Vector2& v1, const Vector2& v2, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR X = XMVectorMax(x1, x2);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Max(const Vector2& v1, const Vector2& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR X = XMVectorMax(x1, x2);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::Lerp(const Vector2& v1, const Vector2& v2, float t, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Lerp(const Vector2& v1, const Vector2& v2, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::SmoothStep(const Vector2& v1, const Vector2& v2, float t, Vector2& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::SmoothStep(const Vector2& v1, const Vector2& v2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR x3 = XMLoadFloat2(&v3);
		XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR x3 = XMLoadFloat2(&v3);
		XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR x3 = XMLoadFloat2(&v3);
		XMVECTOR x4 = XMLoadFloat2(&v4);
		XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&v2);
		XMVECTOR x3 = XMLoadFloat2(&v3);
		XMVECTOR x4 = XMLoadFloat2(&v4);
		XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&t1);
		XMVECTOR x3 = XMLoadFloat2(&v2);
		XMVECTOR x4 = XMLoadFloat2(&t2);
		XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat2(&v1);
		XMVECTOR x2 = XMLoadFloat2(&t1);
		XMVECTOR x3 = XMLoadFloat2(&v2);
		XMVECTOR x4 = XMLoadFloat2(&t2);
		XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::Reflect(const Vector2& ivec, const Vector2& nvec, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat2(&ivec);
		XMVECTOR n = XMLoadFloat2(&nvec);
		XMVECTOR X = XMVector2Reflect(i, n);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Reflect(const Vector2& ivec, const Vector2& nvec)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat2(&ivec);
		XMVECTOR n = XMLoadFloat2(&nvec);
		XMVECTOR X = XMVector2Reflect(i, n);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat2(&ivec);
		XMVECTOR n = XMLoadFloat2(&nvec);
		XMVECTOR X = XMVector2Refract(i, n, refractionIndex);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat2(&ivec);
		XMVECTOR n = XMLoadFloat2(&nvec);
		XMVECTOR X = XMVector2Refract(i, n, refractionIndex);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::Transform(const Vector2& v, const Quaternion& quat, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Transform(const Vector2& v, const Quaternion& quat)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void Vector2::Transform(const Vector2& v, const Matrix& m, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector2TransformCoord(v1, M);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::Transform(const Vector2& v, const Matrix& m)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector2TransformCoord(v1, M);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	_Use_decl_annotations_
		inline void Vector2::Transform(const Vector2* varray, size_t Count, const Matrix& m, Vector2* resultArray)
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVector2TransformCoordStream(resultArray, sizeof(XMFLOAT2), varray, sizeof(XMFLOAT2), Count, M);
	}

	inline void Vector2::Transform(const Vector2& v, const Matrix& m, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector2Transform(v1, M);
		XMStoreFloat4(&result, X);
	}

	_Use_decl_annotations_
		inline void Vector2::Transform(const Vector2* varray, size_t Count, const Matrix& m, Vector4* resultArray)
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVector2TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT2), Count, M);
	}

	inline void Vector2::TransformNormal(const Vector2& v, const Matrix& m, Vector2& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector2TransformNormal(v1, M);
		XMStoreFloat2(&result, X);
	}

	inline Vector2 Vector2::TransformNormal(const Vector2& v, const Matrix& m)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector2TransformNormal(v1, M);

		Vector2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	_Use_decl_annotations_
		inline void Vector2::TransformNormal(const Vector2* varray, size_t Count, const Matrix& m, Vector2* resultArray)
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVector2TransformNormalStream(resultArray, sizeof(XMFLOAT2), varray, sizeof(XMFLOAT2), Count, M);
	}

	/****************************************************************************
	 *
	 * Vector3
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 // Comparision operators
	 //------------------------------------------------------------------------------

	inline bool Vector3::operator == (const Vector3& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		return XMVector3Equal(v1, v2);
	}

	inline bool Vector3::operator != (const Vector3& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		return XMVector3NotEqual(v1, v2);
	}

	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Vector3& Vector3::operator+= (const Vector3& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X = XMVectorAdd(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}

	inline Vector3& Vector3::operator-= (const Vector3& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X = XMVectorSubtract(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}

	inline Vector3& Vector3::operator*= (const Vector3& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X = XMVectorMultiply(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}

	inline Vector3& Vector3::operator*= (float S)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X = XMVectorScale(v1, S);
		XMStoreFloat3(this, X);
		return *this;
	}

	inline Vector3& Vector3::operator/= (float S)
	{
		using namespace DirectX;
		assert(S != 0.0f);
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X = XMVectorScale(v1, 1.f / S);
		XMStoreFloat3(this, X);
		return *this;
	}

	//------------------------------------------------------------------------------
	// Urnary operators
	//------------------------------------------------------------------------------

	inline Vector3 Vector3::operator- () const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X = XMVectorNegate(v1);
		Vector3 R;
		XMStoreFloat3(&R, X);
		return R;
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Vector3 operator+ (const Vector3& V1, const Vector3& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&V1);
		XMVECTOR v2 = XMLoadFloat3(&V2);
		XMVECTOR X = XMVectorAdd(v1, v2);
		Vector3 R;
		XMStoreFloat3(&R, X);
		return R;
	}

	inline Vector3 operator- (const Vector3& V1, const Vector3& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&V1);
		XMVECTOR v2 = XMLoadFloat3(&V2);
		XMVECTOR X = XMVectorSubtract(v1, v2);
		Vector3 R;
		XMStoreFloat3(&R, X);
		return R;
	}

	inline Vector3 operator* (const Vector3& V1, const Vector3& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&V1);
		XMVECTOR v2 = XMLoadFloat3(&V2);
		XMVECTOR X = XMVectorMultiply(v1, v2);
		Vector3 R;
		XMStoreFloat3(&R, X);
		return R;
	}

	inline Vector3 operator* (const Vector3& V, float S)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&V);
		XMVECTOR X = XMVectorScale(v1, S);
		Vector3 R;
		XMStoreFloat3(&R, X);
		return R;
	}

	inline Vector3 operator/ (const Vector3& V1, const Vector3& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&V1);
		XMVECTOR v2 = XMLoadFloat3(&V2);
		XMVECTOR X = XMVectorDivide(v1, v2);
		Vector3 R;
		XMStoreFloat3(&R, X);
		return R;
	}

	inline Vector3 operator* (float S, const Vector3& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&V);
		XMVECTOR X = XMVectorScale(v1, S);
		Vector3 R;
		XMStoreFloat3(&R, X);
		return R;
	}

	//------------------------------------------------------------------------------
	// Vector operations
	//------------------------------------------------------------------------------

	inline bool Vector3::InBounds(const Vector3& Bounds) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&Bounds);
		return XMVector3InBounds(v1, v2);
	}

	inline float Vector3::Length() const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X = XMVector3Length(v1);
		return XMVectorGetX(X);
	}

	inline float Vector3::LengthSquared() const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X = XMVector3LengthSq(v1);
		return XMVectorGetX(X);
	}

	inline float Vector3::Dot(const Vector3& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X = XMVector3Dot(v1, v2);
		return XMVectorGetX(X);
	}

	inline void Vector3::Cross(const Vector3& V, Vector3& result) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR R = XMVector3Cross(v1, v2);
		XMStoreFloat3(&result, R);
	}

	inline Vector3 Vector3::Cross(const Vector3& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR R = XMVector3Cross(v1, v2);

		Vector3 result;
		XMStoreFloat3(&result, R);
		return result;
	}

	inline void Vector3::Normalize()
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X = XMVector3Normalize(v1);
		XMStoreFloat3(this, X);
	}

	inline Vector3 Vector3::Normalize() const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X = XMVector3Normalize(v1);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Normalize(Vector3& result) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X = XMVector3Normalize(v1);
		XMStoreFloat3(&result, X);
	}

	inline void Vector3::Clamp(const Vector3& vmin, const Vector3& vmax)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&vmin);
		XMVECTOR v3 = XMLoadFloat3(&vmax);
		XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat3(this, X);
	}

	inline void Vector3::Clamp(const Vector3& vmin, const Vector3& vmax, Vector3& result) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&vmin);
		XMVECTOR v3 = XMLoadFloat3(&vmax);
		XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat3(&result, X);
	}

	//------------------------------------------------------------------------------
	// Static functions
	//------------------------------------------------------------------------------

	inline float Vector3::Distance(const Vector3& v1, const Vector3& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR V = XMVectorSubtract(x2, x1);
		XMVECTOR X = XMVector3Length(V);
		return XMVectorGetX(X);
	}

	inline float Vector3::DistanceSquared(const Vector3& v1, const Vector3& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR V = XMVectorSubtract(x2, x1);
		XMVECTOR X = XMVector3LengthSq(V);
		return XMVectorGetX(X);
	}

	inline void Vector3::Min(const Vector3& v1, const Vector3& v2, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR X = XMVectorMin(x1, x2);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Min(const Vector3& v1, const Vector3& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR X = XMVectorMin(x1, x2);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Max(const Vector3& v1, const Vector3& v2, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR X = XMVectorMax(x1, x2);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Max(const Vector3& v1, const Vector3& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR X = XMVectorMax(x1, x2);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::SmoothStep(const Vector3& v1, const Vector3& v2, float t, Vector3& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::SmoothStep(const Vector3& v1, const Vector3& v2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR x3 = XMLoadFloat3(&v3);
		XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR x3 = XMLoadFloat3(&v3);
		XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR x3 = XMLoadFloat3(&v3);
		XMVECTOR x4 = XMLoadFloat3(&v4);
		XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&v2);
		XMVECTOR x3 = XMLoadFloat3(&v3);
		XMVECTOR x4 = XMLoadFloat3(&v4);
		XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&t1);
		XMVECTOR x3 = XMLoadFloat3(&v2);
		XMVECTOR x4 = XMLoadFloat3(&t2);
		XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat3(&v1);
		XMVECTOR x2 = XMLoadFloat3(&t1);
		XMVECTOR x3 = XMLoadFloat3(&v2);
		XMVECTOR x4 = XMLoadFloat3(&t2);
		XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Reflect(const Vector3& ivec, const Vector3& nvec, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat3(&ivec);
		XMVECTOR n = XMLoadFloat3(&nvec);
		XMVECTOR X = XMVector3Reflect(i, n);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Reflect(const Vector3& ivec, const Vector3& nvec)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat3(&ivec);
		XMVECTOR n = XMLoadFloat3(&nvec);
		XMVECTOR X = XMVector3Reflect(i, n);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat3(&ivec);
		XMVECTOR n = XMLoadFloat3(&nvec);
		XMVECTOR X = XMVector3Refract(i, n, refractionIndex);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat3(&ivec);
		XMVECTOR n = XMLoadFloat3(&nvec);
		XMVECTOR X = XMVector3Refract(i, n, refractionIndex);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Transform(const Vector3& v, const Quaternion& quat, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Transform(const Vector3& v, const Quaternion& quat)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void Vector3::Transform(const Vector3& v, const Matrix& m, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector3TransformCoord(v1, M);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::Transform(const Vector3& v, const Matrix& m)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector3TransformCoord(v1, M);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	_Use_decl_annotations_
		inline void Vector3::Transform(const Vector3* varray, size_t Count, const Matrix& m, Vector3* resultArray)
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVector3TransformCoordStream(resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), Count, M);
	}

	inline void Vector3::Transform(const Vector3& v, const Matrix& m, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector3Transform(v1, M);
		XMStoreFloat4(&result, X);
	}

	_Use_decl_annotations_
		inline void Vector3::Transform(const Vector3* varray, size_t Count, const Matrix& m, Vector4* resultArray)
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVector3TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT3), Count, M);
	}

	inline void Vector3::TransformNormal(const Vector3& v, const Matrix& m, Vector3& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector3TransformNormal(v1, M);
		XMStoreFloat3(&result, X);
	}

	inline Vector3 Vector3::TransformNormal(const Vector3& v, const Matrix& m)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector3TransformNormal(v1, M);

		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	_Use_decl_annotations_
		inline void Vector3::TransformNormal(const Vector3* varray, size_t Count, const Matrix& m, Vector3* resultArray)
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVector3TransformNormalStream(resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), Count, M);
	}

	inline Vector3 Vector3::Normalize(const Vector3& v)
	{
		using namespace DirectX;
		XMVECTOR V = XMLoadFloat3(&v);
		XMVECTOR X = XMVector3Normalize(V);
		Vector3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	/****************************************************************************
	 *
	 * Vector4
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 // Comparision operators
	 //------------------------------------------------------------------------------

	inline bool Vector4::operator == (const Vector4& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		return XMVector4Equal(v1, v2);
	}

	inline bool Vector4::operator != (const Vector4& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		return XMVector4NotEqual(v1, v2);
	}

	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Vector4& Vector4::operator+= (const Vector4& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		XMVECTOR X = XMVectorAdd(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}

	inline Vector4& Vector4::operator-= (const Vector4& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		XMVECTOR X = XMVectorSubtract(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}

	inline Vector4& Vector4::operator*= (const Vector4& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		XMVECTOR X = XMVectorMultiply(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}

	inline Vector4& Vector4::operator*= (float S)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X = XMVectorScale(v1, S);
		XMStoreFloat4(this, X);
		return *this;
	}

	inline Vector4& Vector4::operator/= (float S)
	{
		using namespace DirectX;
		assert(S != 0.0f);
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X = XMVectorScale(v1, 1.f / S);
		XMStoreFloat4(this, X);
		return *this;
	}

	//------------------------------------------------------------------------------
	// Urnary operators
	//------------------------------------------------------------------------------

	inline Vector4 Vector4::operator- () const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X = XMVectorNegate(v1);
		Vector4 R;
		XMStoreFloat4(&R, X);
		return R;
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Vector4 operator+ (const Vector4& V1, const Vector4& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&V1);
		XMVECTOR v2 = XMLoadFloat4(&V2);
		XMVECTOR X = XMVectorAdd(v1, v2);
		Vector4 R;
		XMStoreFloat4(&R, X);
		return R;
	}

	inline Vector4 operator- (const Vector4& V1, const Vector4& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&V1);
		XMVECTOR v2 = XMLoadFloat4(&V2);
		XMVECTOR X = XMVectorSubtract(v1, v2);
		Vector4 R;
		XMStoreFloat4(&R, X);
		return R;
	}

	inline Vector4 operator* (const Vector4& V1, const Vector4& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&V1);
		XMVECTOR v2 = XMLoadFloat4(&V2);
		XMVECTOR X = XMVectorMultiply(v1, v2);
		Vector4 R;
		XMStoreFloat4(&R, X);
		return R;
	}

	inline Vector4 operator* (const Vector4& V, float S)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&V);
		XMVECTOR X = XMVectorScale(v1, S);
		Vector4 R;
		XMStoreFloat4(&R, X);
		return R;
	}

	inline Vector4 operator/ (const Vector4& V1, const Vector4& V2)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&V1);
		XMVECTOR v2 = XMLoadFloat4(&V2);
		XMVECTOR X = XMVectorDivide(v1, v2);
		Vector4 R;
		XMStoreFloat4(&R, X);
		return R;
	}

	inline Vector4 operator* (float S, const Vector4& V)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&V);
		XMVECTOR X = XMVectorScale(v1, S);
		Vector4 R;
		XMStoreFloat4(&R, X);
		return R;
	}

	//------------------------------------------------------------------------------
	// Vector operations
	//------------------------------------------------------------------------------

	inline bool Vector4::InBounds(const Vector4& Bounds) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&Bounds);
		return XMVector4InBounds(v1, v2);
	}

	inline float Vector4::Length() const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X = XMVector4Length(v1);
		return XMVectorGetX(X);
	}

	inline float Vector4::LengthSquared() const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X = XMVector4LengthSq(v1);
		return XMVectorGetX(X);
	}

	inline float Vector4::Dot(const Vector4& V) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		XMVECTOR X = XMVector4Dot(v1, v2);
		return XMVectorGetX(X);
	}

	inline void Vector4::Cross(const Vector4& v1, const Vector4& v2, Vector4& result) const
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(this);
		XMVECTOR x2 = XMLoadFloat4(&v1);
		XMVECTOR x3 = XMLoadFloat4(&v2);
		XMVECTOR R = XMVector4Cross(x1, x2, x3);
		XMStoreFloat4(&result, R);
	}

	inline Vector4 Vector4::Cross(const Vector4& v1, const Vector4& v2) const
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(this);
		XMVECTOR x2 = XMLoadFloat4(&v1);
		XMVECTOR x3 = XMLoadFloat4(&v2);
		XMVECTOR R = XMVector4Cross(x1, x2, x3);

		Vector4 result;
		XMStoreFloat4(&result, R);
		return result;
	}

	inline void Vector4::Normalize()
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X = XMVector4Normalize(v1);
		XMStoreFloat4(this, X);
	}

	inline void Vector4::Normalize(Vector4& result) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X = XMVector4Normalize(v1);
		XMStoreFloat4(&result, X);
	}

	inline void Vector4::Clamp(const Vector4& vmin, const Vector4& vmax)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&vmin);
		XMVECTOR v3 = XMLoadFloat4(&vmax);
		XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat4(this, X);
	}

	inline void Vector4::Clamp(const Vector4& vmin, const Vector4& vmax, Vector4& result) const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&vmin);
		XMVECTOR v3 = XMLoadFloat4(&vmax);
		XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat4(&result, X);
	}

	//------------------------------------------------------------------------------
	// Static functions
	//------------------------------------------------------------------------------

	inline float Vector4::Distance(const Vector4& v1, const Vector4& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR V = XMVectorSubtract(x2, x1);
		XMVECTOR X = XMVector4Length(V);
		return XMVectorGetX(X);
	}

	inline float Vector4::DistanceSquared(const Vector4& v1, const Vector4& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR V = XMVectorSubtract(x2, x1);
		XMVECTOR X = XMVector4LengthSq(V);
		return XMVectorGetX(X);
	}

	inline void Vector4::Min(const Vector4& v1, const Vector4& v2, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR X = XMVectorMin(x1, x2);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Min(const Vector4& v1, const Vector4& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR X = XMVectorMin(x1, x2);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Max(const Vector4& v1, const Vector4& v2, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR X = XMVectorMax(x1, x2);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Max(const Vector4& v1, const Vector4& v2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR X = XMVectorMax(x1, x2);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Lerp(const Vector4& v1, const Vector4& v2, float t, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Lerp(const Vector4& v1, const Vector4& v2, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::SmoothStep(const Vector4& v1, const Vector4& v2, float t, Vector4& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::SmoothStep(const Vector4& v1, const Vector4& v2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR X = XMVectorLerp(x1, x2, t);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR x3 = XMLoadFloat4(&v3);
		XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR x3 = XMLoadFloat4(&v3);
		XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR x3 = XMLoadFloat4(&v3);
		XMVECTOR x4 = XMLoadFloat4(&v4);
		XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&v2);
		XMVECTOR x3 = XMLoadFloat4(&v3);
		XMVECTOR x4 = XMLoadFloat4(&v4);
		XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&t1);
		XMVECTOR x3 = XMLoadFloat4(&v2);
		XMVECTOR x4 = XMLoadFloat4(&t2);
		XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(&v1);
		XMVECTOR x2 = XMLoadFloat4(&t1);
		XMVECTOR x3 = XMLoadFloat4(&v2);
		XMVECTOR x4 = XMLoadFloat4(&t2);
		XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Reflect(const Vector4& ivec, const Vector4& nvec, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat4(&ivec);
		XMVECTOR n = XMLoadFloat4(&nvec);
		XMVECTOR X = XMVector4Reflect(i, n);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Reflect(const Vector4& ivec, const Vector4& nvec)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat4(&ivec);
		XMVECTOR n = XMLoadFloat4(&nvec);
		XMVECTOR X = XMVector4Reflect(i, n);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat4(&ivec);
		XMVECTOR n = XMLoadFloat4(&nvec);
		XMVECTOR X = XMVector4Refract(i, n, refractionIndex);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex)
	{
		using namespace DirectX;
		XMVECTOR i = XMLoadFloat4(&ivec);
		XMVECTOR n = XMLoadFloat4(&nvec);
		XMVECTOR X = XMVector4Refract(i, n, refractionIndex);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Transform(const Vector2& v, const Quaternion& quat, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);
		X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110); // result.w = 1.f
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Transform(const Vector2& v, const Quaternion& quat)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);
		X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110); // result.w = 1.f

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Transform(const Vector3& v, const Quaternion& quat, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);
		X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110); // result.w = 1.f
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Transform(const Vector3& v, const Quaternion& quat)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);
		X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110); // result.w = 1.f

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Transform(const Vector4& v, const Quaternion& quat, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);
		X = XMVectorSelect(v1, X, g_XMSelect1110); // result.w = v.w
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Transform(const Vector4& v, const Quaternion& quat)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&v);
		XMVECTOR q = XMLoadFloat4(&quat);
		XMVECTOR X = XMVector3Rotate(v1, q);
		X = XMVectorSelect(v1, X, g_XMSelect1110); // result.w = v.w

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void Vector4::Transform(const Vector4& v, const Matrix& m, Vector4& result)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector4Transform(v1, M);
		XMStoreFloat4(&result, X);
	}

	inline Vector4 Vector4::Transform(const Vector4& v, const Matrix& m)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(&v);
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVECTOR X = XMVector4Transform(v1, M);

		Vector4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	_Use_decl_annotations_
		inline void Vector4::Transform(const Vector4* varray, size_t Count, const Matrix& m, Vector4* resultArray)
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(&m);
		XMVector4TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT4), Count, M);
	}

	/****************************************************************************
	 *
	 * Matrix
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 // Comparision operators
	 //------------------------------------------------------------------------------

	inline bool Matrix::operator == (const Matrix& M) const
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		return (XMVector4Equal(x1, y1)
			&& XMVector4Equal(x2, y2)
			&& XMVector4Equal(x3, y3)
			&& XMVector4Equal(x4, y4)) != 0;
	}

	inline bool Matrix::operator != (const Matrix& M) const
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		return (XMVector4NotEqual(x1, y1)
			|| XMVector4NotEqual(x2, y2)
			|| XMVector4NotEqual(x3, y3)
			|| XMVector4NotEqual(x4, y4)) != 0;
	}

	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Matrix::Matrix(const DirectX::XMFLOAT3X3& M)
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
		_41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
	}

	inline Matrix::Matrix(const DirectX::XMFLOAT4X3& M)
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
		_41 = M._41; _42 = M._42; _43 = M._43; _44 = 1.f;
	}

	inline Matrix& Matrix::operator= (const DirectX::XMFLOAT3X3& M)
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
		_41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
		return *this;
	}

	inline Matrix& Matrix::operator= (const DirectX::XMFLOAT4X3& M)
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
		_41 = M._41; _42 = M._42; _43 = M._43; _44 = 1.f;
		return *this;
	}

	inline Matrix& Matrix::operator+= (const Matrix& M)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorAdd(x1, y1);
		x2 = XMVectorAdd(x2, y2);
		x3 = XMVectorAdd(x3, y3);
		x4 = XMVectorAdd(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	inline Matrix& Matrix::operator-= (const Matrix& M)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorSubtract(x1, y1);
		x2 = XMVectorSubtract(x2, y2);
		x3 = XMVectorSubtract(x3, y3);
		x4 = XMVectorSubtract(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	inline Matrix& Matrix::operator*= (const Matrix& M)
	{
		using namespace DirectX;
		XMMATRIX M1 = XMLoadFloat4x4(this);
		XMMATRIX M2 = XMLoadFloat4x4(&M);
		XMMATRIX X = XMMatrixMultiply(M1, M2);
		XMStoreFloat4x4(this, X);
		return *this;
	}

	inline Matrix& Matrix::operator*= (float S)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		x1 = XMVectorScale(x1, S);
		x2 = XMVectorScale(x2, S);
		x3 = XMVectorScale(x3, S);
		x4 = XMVectorScale(x4, S);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	inline Matrix& Matrix::operator/= (float S)
	{
		using namespace DirectX;
		assert(S != 0.f);
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		float rs = 1.f / S;

		x1 = XMVectorScale(x1, rs);
		x2 = XMVectorScale(x2, rs);
		x3 = XMVectorScale(x3, rs);
		x4 = XMVectorScale(x4, rs);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	inline Matrix& Matrix::operator/= (const Matrix& M)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorDivide(x1, y1);
		x2 = XMVectorDivide(x2, y2);
		x3 = XMVectorDivide(x3, y3);
		x4 = XMVectorDivide(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	//------------------------------------------------------------------------------
	// Urnary operators
	//------------------------------------------------------------------------------

	inline Matrix Matrix::operator- () const
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR v2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR v3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR v4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		v1 = XMVectorNegate(v1);
		v2 = XMVectorNegate(v2);
		v3 = XMVectorNegate(v3);
		v4 = XMVectorNegate(v4);

		Matrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), v1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), v2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), v3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), v4);
		return R;
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Matrix operator+ (const Matrix& M1, const Matrix& M2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorAdd(x1, y1);
		x2 = XMVectorAdd(x2, y2);
		x3 = XMVectorAdd(x3, y3);
		x4 = XMVectorAdd(x4, y4);

		Matrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}

	inline Matrix operator- (const Matrix& M1, const Matrix& M2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorSubtract(x1, y1);
		x2 = XMVectorSubtract(x2, y2);
		x3 = XMVectorSubtract(x3, y3);
		x4 = XMVectorSubtract(x4, y4);

		Matrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}

	inline Matrix operator* (const Matrix& M1, const Matrix& M2)
	{
		using namespace DirectX;
		XMMATRIX m1 = XMLoadFloat4x4(&M1);
		XMMATRIX m2 = XMLoadFloat4x4(&M2);
		XMMATRIX X = XMMatrixMultiply(m1, m2);

		Matrix R;
		XMStoreFloat4x4(&R, X);
		return R;
	}

	inline Matrix operator* (const Matrix& M, float S)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorScale(x1, S);
		x2 = XMVectorScale(x2, S);
		x3 = XMVectorScale(x3, S);
		x4 = XMVectorScale(x4, S);

		Matrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}

	inline Matrix operator/ (const Matrix& M, float S)
	{
		using namespace DirectX;
		assert(S != 0.f);

		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		float rs = 1.f / S;

		x1 = XMVectorScale(x1, rs);
		x2 = XMVectorScale(x2, rs);
		x3 = XMVectorScale(x3, rs);
		x4 = XMVectorScale(x4, rs);

		Matrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}

	inline Matrix operator/ (const Matrix& M1, const Matrix& M2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorDivide(x1, y1);
		x2 = XMVectorDivide(x2, y2);
		x3 = XMVectorDivide(x3, y3);
		x4 = XMVectorDivide(x4, y4);

		Matrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}

	inline Matrix operator* (float S, const Matrix& M)
	{
		using namespace DirectX;

		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorScale(x1, S);
		x2 = XMVectorScale(x2, S);
		x3 = XMVectorScale(x3, S);
		x4 = XMVectorScale(x4, S);

		Matrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}

	//------------------------------------------------------------------------------
	// Matrix operations
	//------------------------------------------------------------------------------

	inline bool Matrix::Decompose(Vector3& scale_, Quaternion& rotation_, Vector3& translation_)
	{
		using namespace DirectX;

		XMVECTOR s, r, t;

		if (!XMMatrixDecompose(&s, &r, &t, *this))
			return false;

		XMStoreFloat3(&scale_, s);
		XMStoreFloat4(&rotation_, r);
		XMStoreFloat3(&translation_, t);

		return true;
	}

	inline Matrix Matrix::Transpose() const
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(this);
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixTranspose(M));
		return R;
	}

	inline void Matrix::Transpose(Matrix& result) const
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(this);
		XMStoreFloat4x4(&result, XMMatrixTranspose(M));
	}

	inline Matrix Matrix::Invert() const
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(this);
		Matrix R;
		XMVECTOR det;
		XMStoreFloat4x4(&R, XMMatrixInverse(&det, M));
		return R;
	}

	inline void Matrix::Invert(Matrix& result) const
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(this);
		XMVECTOR det;
		XMStoreFloat4x4(&result, XMMatrixInverse(&det, M));
	}

	inline float Matrix::Determinant() const
	{
		using namespace DirectX;
		XMMATRIX M = XMLoadFloat4x4(this);
		return XMVectorGetX(XMMatrixDeterminant(M));
	}

	//------------------------------------------------------------------------------
	// Static functions
	//------------------------------------------------------------------------------

	_Use_decl_annotations_
		inline Matrix Matrix::CreateBillboard(const Vector3& object, const Vector3& cameraPosition, const Vector3& cameraUp, const Vector3* cameraForward)
	{
		using namespace DirectX;
		XMVECTOR O = XMLoadFloat3(&object);
		XMVECTOR C = XMLoadFloat3(&cameraPosition);
		XMVECTOR Z = XMVectorSubtract(O, C);

		XMVECTOR N = XMVector3LengthSq(Z);
		if (XMVector3Less(N, g_XMEpsilon))
		{
			if (cameraForward)
			{
				XMVECTOR F = XMLoadFloat3(cameraForward);
				Z = XMVectorNegate(F);
			}
			else
				Z = g_XMNegIdentityR2;
		}
		else
		{
			Z = XMVector3Normalize(Z);
		}

		XMVECTOR up = XMLoadFloat3(&cameraUp);
		XMVECTOR X = XMVector3Cross(up, Z);
		X = XMVector3Normalize(X);

		XMVECTOR Y = XMVector3Cross(Z, X);

		XMMATRIX M;
		M.r[0] = X;
		M.r[1] = Y;
		M.r[2] = Z;
		M.r[3] = XMVectorSetW(O, 1.f);

		Matrix R;
		XMStoreFloat4x4(&R, M);
		return R;
	}

	_Use_decl_annotations_
		inline Matrix Matrix::CreateConstrainedBillboard(const Vector3& object, const Vector3& cameraPosition, const Vector3& rotateAxis,
			const Vector3* cameraForward, const Vector3* objectForward)
	{
		using namespace DirectX;

		static const XMVECTORF32 s_minAngle = { { { 0.99825467075f, 0.99825467075f, 0.99825467075f, 0.99825467075f } } }; // 1.0 - XMConvertToRadians( 0.1f );

		XMVECTOR O = XMLoadFloat3(&object);
		XMVECTOR C = XMLoadFloat3(&cameraPosition);
		XMVECTOR faceDir = XMVectorSubtract(O, C);

		XMVECTOR N = XMVector3LengthSq(faceDir);
		if (XMVector3Less(N, g_XMEpsilon))
		{
			if (cameraForward)
			{
				XMVECTOR F = XMLoadFloat3(cameraForward);
				faceDir = XMVectorNegate(F);
			}
			else
				faceDir = g_XMNegIdentityR2;
		}
		else
		{
			faceDir = XMVector3Normalize(faceDir);
		}

		XMVECTOR Y = XMLoadFloat3(&rotateAxis);
		XMVECTOR X, Z;

		XMVECTOR dot = XMVectorAbs(XMVector3Dot(Y, faceDir));
		if (XMVector3Greater(dot, s_minAngle))
		{
			if (objectForward)
			{
				Z = XMLoadFloat3(objectForward);
				dot = XMVectorAbs(XMVector3Dot(Y, Z));
				if (XMVector3Greater(dot, s_minAngle))
				{
					dot = XMVectorAbs(XMVector3Dot(Y, g_XMNegIdentityR2));
					Z = (XMVector3Greater(dot, s_minAngle)) ? g_XMIdentityR0 : g_XMNegIdentityR2;
				}
			}
			else
			{
				dot = XMVectorAbs(XMVector3Dot(Y, g_XMNegIdentityR2));
				Z = (XMVector3Greater(dot, s_minAngle)) ? g_XMIdentityR0 : g_XMNegIdentityR2;
			}

			X = XMVector3Cross(Y, Z);
			X = XMVector3Normalize(X);

			Z = XMVector3Cross(X, Y);
			Z = XMVector3Normalize(Z);
		}
		else
		{
			X = XMVector3Cross(Y, faceDir);
			X = XMVector3Normalize(X);

			Z = XMVector3Cross(X, Y);
			Z = XMVector3Normalize(Z);
		}

		XMMATRIX M;
		M.r[0] = X;
		M.r[1] = Y;
		M.r[2] = Z;
		M.r[3] = XMVectorSetW(O, 1.f);

		Matrix R;
		XMStoreFloat4x4(&R, M);
		return R;
	}

	inline Matrix Matrix::CreateTranslation(const Vector3& position_)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixTranslation(position_.x, position_.y, position_.z));
		return R;
	}

	inline Matrix Matrix::CreateTranslation(float x, float y, float z)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixTranslation(x, y, z));
		return R;
	}

	inline Matrix Matrix::CreateScale(const Vector3& scales)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixScaling(scales.x, scales.y, scales.z));
		return R;
	}

	inline Matrix Matrix::CreateScale(float xs, float ys, float zs)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixScaling(xs, ys, zs));
		return R;
	}

	inline Matrix Matrix::CreateScale(float scale_)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixScaling(scale_, scale_, scale_));
		return R;
	}

	inline Matrix Matrix::CreateRotationX(float radians)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixRotationX(radians));
		return R;
	}

	inline Matrix Matrix::CreateRotationY(float radians)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixRotationY(radians));
		return R;
	}

	inline Matrix Matrix::CreateRotationZ(float radians)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixRotationZ(radians));
		return R;
	}

	inline Matrix Matrix::CreateFromAxisAngle(const Vector3& axis, float angle)
	{
		using namespace DirectX;
		Matrix R;
		XMVECTOR a = XMLoadFloat3(&axis);
		XMStoreFloat4x4(&R, XMMatrixRotationAxis(a, angle));
		return R;
	}

	inline Matrix Matrix::CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane));
		return R;
	}

	inline Matrix Matrix::CreatePerspective(float width, float height_, float nearPlane, float farPlane)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixPerspectiveRH(width, height_, nearPlane, farPlane));
		return R;
	}

	inline Matrix Matrix::CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlane, float farPlane)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixPerspectiveOffCenterRH(left, right, bottom, top, nearPlane, farPlane));
		return R;
	}

	inline Matrix Matrix::CreateOrthographic(float width, float height_, float zNearPlane, float zFarPlane)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixOrthographicRH(width, height_, zNearPlane, zFarPlane));
		return R;
	}

	inline Matrix Matrix::CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNearPlane, zFarPlane));
		return R;
	}

	inline Matrix Matrix::CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
	{
		using namespace DirectX;
		Matrix R;
		XMVECTOR eyev = XMLoadFloat3(&eye);
		XMVECTOR targetv = XMLoadFloat3(&target);
		XMVECTOR upv = XMLoadFloat3(&up);
		XMStoreFloat4x4(&R, XMMatrixLookAtRH(eyev, targetv, upv));
		return R;
	}

	inline Matrix Matrix::CreateWorld(const Vector3& position_, const Vector3& forward, const Vector3& up)
	{
		using namespace DirectX;
		XMVECTOR zaxis = XMVector3Normalize(XMVectorNegate(XMLoadFloat3(&forward)));
		XMVECTOR yaxis = XMLoadFloat3(&up);
		XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(yaxis, zaxis));
		yaxis = XMVector3Cross(zaxis, xaxis);

		Matrix R;
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), xaxis);
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), yaxis);
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), zaxis);
		R._14 = R._24 = R._34 = 0.f;
		R._41 = position_.x; R._42 = position_.y; R._43 = position_.z;
		R._44 = 1.f;
		return R;
	}

	inline Matrix Matrix::CreateFromQuaternion(const Quaternion& rotation_)
	{
		using namespace DirectX;
		Matrix R;
		XMVECTOR quatv = XMLoadFloat4(&rotation_);
		XMStoreFloat4x4(&R, XMMatrixRotationQuaternion(quatv));
		return R;
	}

	inline Matrix Matrix::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
	{
		using namespace DirectX;
		Matrix R;
		XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
		return R;
	}
	/*
	inline Matrix Matrix::CreateShadow(const Vector3& lightDir, const Plane& plane)
	{
		using namespace DirectX;
		Matrix R;
		XMVECTOR light = XMLoadFloat3(&lightDir);
		XMVECTOR planev = XMLoadFloat4(&plane);
		XMStoreFloat4x4(&R, XMMatrixShadow(planev, light));
		return R;
	}

	inline Matrix Matrix::CreateReflection(const Plane& plane)
	{
		using namespace DirectX;
		Matrix R;
		XMVECTOR planev = XMLoadFloat4(&plane);
		XMStoreFloat4x4(&R, XMMatrixReflect(planev));
		return R;
	}
	*/
	inline void Matrix::Lerp(const Matrix& M1, const Matrix& M2, float t, Matrix& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorLerp(x1, y1, t);
		x2 = XMVectorLerp(x2, y2, t);
		x3 = XMVectorLerp(x3, y3, t);
		x4 = XMVectorLerp(x4, y4, t);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
	}

	inline Matrix Matrix::Lerp(const Matrix& M1, const Matrix& M2, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorLerp(x1, y1, t);
		x2 = XMVectorLerp(x2, y2, t);
		x3 = XMVectorLerp(x3, y3, t);
		x4 = XMVectorLerp(x4, y4, t);

		Matrix result;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
		return result;
	}

	inline void Matrix::Transform(const Matrix& M, const Quaternion& rotation_, Matrix& result)
	{
		using namespace DirectX;
		XMVECTOR quatv = XMLoadFloat4(&rotation_);

		XMMATRIX M0 = XMLoadFloat4x4(&M);
		XMMATRIX M1 = XMMatrixRotationQuaternion(quatv);

		XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
	}

	inline Matrix Matrix::Transform(const Matrix& M, const Quaternion& rotation_)
	{
		using namespace DirectX;
		XMVECTOR quatv = XMLoadFloat4(&rotation_);

		XMMATRIX M0 = XMLoadFloat4x4(&M);
		XMMATRIX M1 = XMMatrixRotationQuaternion(quatv);

		Matrix result;
		XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
		return result;
	}

	/****************************************************************************
	 *
	 * Quaternion
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 // Comparision operators
	 //------------------------------------------------------------------------------

	inline bool Quaternion::operator == (const Quaternion& q) const
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		return XMQuaternionEqual(q1, q2);
	}

	inline bool Quaternion::operator != (const Quaternion& q) const
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		return XMQuaternionNotEqual(q1, q2);
	}

	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Quaternion& Quaternion::operator+= (const Quaternion& q)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		XMVECTOR v = XMVectorAdd(q1, q2);
		XMStoreFloat4(this, v);


		return *this;
	}

	inline Quaternion& Quaternion::operator-= (const Quaternion& q)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		XMStoreFloat4(this, XMVectorSubtract(q1, q2));

		return *this;
	}

	inline Quaternion& Quaternion::operator*= (const Quaternion& q)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		XMStoreFloat4(this, XMQuaternionMultiply(q1, q2));

		return *this;
	}

	inline Quaternion& Quaternion::operator*= (float S)
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(this, XMVectorScale(q, S));

		return *this;
	}

	inline Quaternion& Quaternion::operator/= (const Quaternion& q)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		q2 = XMQuaternionInverse(q2);
		XMStoreFloat4(this, XMQuaternionMultiply(q1, q2));

		return *this;
	}

	//------------------------------------------------------------------------------
	// Urnary operators
	//------------------------------------------------------------------------------

	inline Quaternion Quaternion::operator- () const
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);

		Quaternion R;
		XMStoreFloat4(&R, XMVectorNegate(q));

		return R;
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Quaternion operator+ (const Quaternion& Q1, const Quaternion& Q2)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(&Q1);
		XMVECTOR q2 = XMLoadFloat4(&Q2);

		Quaternion R;
		XMStoreFloat4(&R, XMVectorAdd(q1, q2));

		return R;
	}

	inline Quaternion operator- (const Quaternion& Q1, const Quaternion& Q2)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(&Q1);
		XMVECTOR q2 = XMLoadFloat4(&Q2);

		Quaternion R;
		XMStoreFloat4(&R, XMVectorSubtract(q1, q2));

		return R;
	}

	inline Quaternion operator* (const Quaternion& Q1, const Quaternion& Q2)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(&Q1);
		XMVECTOR q2 = XMLoadFloat4(&Q2);

		Quaternion R;
		XMStoreFloat4(&R, XMQuaternionMultiply(q1, q2));

		return R;
	}

	inline Quaternion operator* (const Quaternion& Q, float S)
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(&Q);

		Quaternion R;
		XMStoreFloat4(&R, XMVectorScale(q, S));

		return R;
	}

	inline Quaternion operator/ (const Quaternion& Q1, const Quaternion& Q2)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(&Q1);
		XMVECTOR q2 = XMLoadFloat4(&Q2);
		q2 = XMQuaternionInverse(q2);

		Quaternion R;
		XMStoreFloat4(&R, XMQuaternionMultiply(q1, q2));

		return R;
	}

	inline Quaternion operator* (float S, const Quaternion& Q)
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(&Q);

		Quaternion R;
		XMStoreFloat4(&R, XMVectorScale(q1, S));

		return R;
	}

	//------------------------------------------------------------------------------
	// Quaternion operations
	//------------------------------------------------------------------------------

	inline float Quaternion::Length() const
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);
		return XMVectorGetX(XMQuaternionLength(q));
	}

	inline float Quaternion::LengthSquared() const
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);
		return XMVectorGetX(XMQuaternionLengthSq(q));
	}

	inline void Quaternion::Normalize()
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(this, XMQuaternionNormalize(q));

	}

	inline void Quaternion::Normalize(Quaternion& result) const
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(&result, XMQuaternionNormalize(q));;

	}

	inline void Quaternion::Conjugate()
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(this, XMQuaternionConjugate(q));

	}

	inline void Quaternion::Conjugate(Quaternion& result) const
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(&result, XMQuaternionConjugate(q));;

	}

	inline void Quaternion::Inverse(Quaternion& result) const
	{
		using namespace DirectX;
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(&result, XMQuaternionInverse(q));

	}

	inline float Quaternion::Dot(const Quaternion& q) const
	{
		using namespace DirectX;
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		return XMVectorGetX(XMQuaternionDot(q1, q2));
	}

	inline Vector3 Quaternion::To_Euler() const
	{
		using namespace DirectX;
		Vector3 R = { 0,0,0 };
		Matrix M = Matrix::CreateFromQuaternion(*this);

		if (M._32 >= 1.0f)
		{
			R.x = XM_PIDIV2;
			R.y = 0;
			R.z = atan2f(M._21, M._11);
		}
		else if (M._32 <= -1.0f)
		{
			R.x = -XM_PIDIV2;
			R.y = 0;
			R.z = atan2f(M._21, M._11);
		}
		else
		{
			R.x = asinf(-M._32);
			R.y = atan2f(M._31, M._33);
			R.z = atan2f(M._12, M._22);
		}

		R.x = XMConvertToDegrees(R.x);
		R.y = XMConvertToDegrees(R.y);
		R.z = XMConvertToDegrees(R.z);
		return R;
	}
	inline void Quaternion::Set_Euler(Vector3 V)
	{
		Quaternion Q = Quaternion::Euler(V);
		XMStoreFloat4(this, XMLoadFloat4(&Q));
	}

	//------------------------------------------------------------------------------
	// Static functions
	//------------------------------------------------------------------------------

	inline Quaternion Quaternion::AngleAxis(const Vector3& axis, float angle)
	{
		using namespace DirectX;
		XMVECTOR a = XMLoadFloat3(&axis);

		Quaternion R;
		XMStoreFloat4(&R, XMQuaternionRotationAxis(a, angle));

		return R;
	}

	inline Quaternion Quaternion::Euler(float x, float y, float z)
	{
		using namespace DirectX;
		Quaternion R;
		XMStoreFloat4(&R, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
		return R;
	}
	inline Quaternion Quaternion::Euler(Vector3 V)
	{
		using namespace DirectX;
		Quaternion R;
		XMStoreFloat4(&R, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(V.x), XMConvertToRadians(V.y), XMConvertToRadians(V.z)));
		return R;
	}

	inline Quaternion Quaternion::CreateFromRotationMatrix(const Matrix& M)
	{
		using namespace DirectX;
		XMMATRIX M0 = XMLoadFloat4x4(&M);

		Quaternion R;
		XMStoreFloat4(&R, XMQuaternionRotationMatrix(M0));

		return R;
	}

	inline void Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result)
	{
		using namespace DirectX;
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		XMVECTOR dot = XMVector4Dot(Q0, Q1);

		XMVECTOR R;
		if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
		{
			R = XMVectorLerp(Q0, Q1, t);
		}
		else
		{
			XMVECTOR tv = XMVectorReplicate(t);
			XMVECTOR t1v = XMVectorReplicate(1.f - t);
			XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
			XMVECTOR X1 = XMVectorMultiply(Q1, tv);
			R = XMVectorSubtract(X0, X1);
		}

		XMStoreFloat4(&result, XMQuaternionNormalize(R));

	}

	inline Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		using namespace DirectX;
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		XMVECTOR dot = XMVector4Dot(Q0, Q1);

		XMVECTOR R;
		if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
		{
			R = XMVectorLerp(Q0, Q1, t);
		}
		else
		{
			XMVECTOR tv = XMVectorReplicate(t);
			XMVECTOR t1v = XMVectorReplicate(1.f - t);
			XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
			XMVECTOR X1 = XMVectorMultiply(Q1, tv);
			R = XMVectorSubtract(X0, X1);
		}

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(R));

		return result;
	}

	inline void Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result)
	{
		using namespace DirectX;
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);
		XMStoreFloat4(&result, XMQuaternionSlerp(Q0, Q1, t));

	}

	inline Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		using namespace DirectX;
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionSlerp(Q0, Q1, t));

		return result;
	}

	inline void Quaternion::Concatenate(const Quaternion& q1, const Quaternion& q2, Quaternion& result)
	{
		using namespace DirectX;
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);
		XMStoreFloat4(&result, XMQuaternionMultiply(Q1, Q0));

	}

	inline Quaternion Quaternion::Concatenate(const Quaternion& q1, const Quaternion& q2)
	{
		using namespace DirectX;
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionMultiply(Q1, Q0));

		return result;
	}
}
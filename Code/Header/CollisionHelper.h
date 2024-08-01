#pragma once
#pragma once

#include <DirectXMath.h>
#include <cmath>
#include <algorithm>


namespace AbyssEngine
{
	// �q�b�g����
	struct HitResult
	{
		DirectX::XMFLOAT3	position = { 0, 0, 0 };// ���C�ƃ|���S���̌�_
		DirectX::XMFLOAT3	normal = { 0, 0, 0 };	// �Փ˂����|���S���̖@���x�N�g��
		float				distance = 0.0f; 		// ���C�̎n�_�����_�܂ł̋���
		int					materialIndex = -1; 	// �Փ˂����|���S���̃}�e���A���ԍ�
		DirectX::XMFLOAT3	triangleVerts[3] = {};
	};
	struct HitResultVector
	{
		DirectX::XMVECTOR	position = { 0, 0, 0 };// ���C�ƃ|���S���̌�_
		DirectX::XMVECTOR	normal = { 0, 0, 0 };	// �Փ˂����|���S���̖@���x�N�g��
		float				distance = 0.0f; 		// ���C�̎n�_�����_�܂ł̋���
		DirectX::XMVECTOR	triangleVerts[3] = {};
		int					materialIndex = -1; 	// �Փ˂����|���S���̃}�e���A���ԍ�
	};
	// �I�u�W�F�N�g�̌������ʂ̍\����
	struct IntersectionResult
	{
		DirectX::XMVECTOR	pointA = {};			// �I�u�W�F�N�gA���̌�_�B�I�u�W�F�N�gA Vs �I�u�W�F�N�gB�Ŋ֐����\�������
		DirectX::XMVECTOR	pointB = {};			// �I�u�W�F�N�gB���̌�_�B�I�u�W�F�N�gA Vs �I�u�W�F�N�gB�Ŋ֐����\�������
		DirectX::XMVECTOR	normal = {};			// ��_�����ԏՓ˂̒P�ʖ@���x�N�g���B�����̓I�u�W�F�N�gB���I�u�W�F�N�gA
		float				penetration = 0.0f; 	// �@���x�N�g�������ɂ����߂荞�ݗʁB�������Ă���ꍇ�Ƀv���X�̒l���ԋp�����
	};

	struct Triangle
	{
		DirectX::XMFLOAT3	position[3] = {};
		int					materialIndex = -1;
		uint32_t			quadTreeIndex = 0;		// �l���؋�Ԃ̓o�^��ԃC���f�b�N�X
	};
	struct Sphere
	{
		DirectX::XMFLOAT3	position;	// ���S
		float				radius;	// ���a
		uint32_t			quadTreeIndex = 0;		// �l���؋�Ԃ̓o�^��ԃC���f�b�N�X
	};
	struct AABB
	{
		DirectX::XMFLOAT3	position;	// ���S
		DirectX::XMFLOAT3	radii;	// ���a
		uint32_t			quadTreeIndex = 0;		// �l���؋�Ԃ̓o�^��ԃC���f�b�N�X
	};
	struct Capsule
	{
		DirectX::XMFLOAT3	position;	// ���S
		DirectX::XMFLOAT3	direction;	// �����i���K���j
		float				length;	// ����
		float				radius;	// ���a
		uint32_t			quadTreeIndex = 0;		// �l���؋�Ԃ̓o�^��ԃC���f�b�N�X
	};

	// �R���W����
	class Collision
	{
	public:
		static bool IntersectRayVsTriangle(
			const DirectX::XMVECTOR& rayStart,
			const DirectX::XMVECTOR& rayDirection,		// �v���K��
			float rayDist,
			const DirectX::XMVECTOR triangleVerts[3],
			HitResultVector& result);

		static bool IntersectRayVsTriangle(
			const DirectX::XMVECTOR& rayStart,
			const DirectX::XMVECTOR& rayDirection,		// �񐳋K��
			const DirectX::XMVECTOR triangleVerts[3],
			HitResultVector& result);

		static bool IntersectRayVsTriangle(
			const DirectX::XMFLOAT3& rayStart,
			const DirectX::XMFLOAT3& rayDirection,		// �v���K��
			float rayDist,
			const DirectX::XMFLOAT3 triangleVerts[3],
			HitResult& result);

		// �O���̓_�ɑ΂���O�p�`�����̍ŋߓ_���擾����
		static bool GetClosestPoint_PointTriangle(
			const DirectX::XMVECTOR& point,
			const DirectX::XMVECTOR trianglePos[3],
			DirectX::XMVECTOR& nearPos);

		// ���Ƌ��̌�������
		static bool IntersectSphereVsSphere(
			const DirectX::XMVECTOR& position1,
			float radius1,
			const DirectX::XMVECTOR& position2,
			float radius2,
			IntersectionResult* result
		);
		// ��Vs�O�p�`
		static bool IntersectSphereVsTriangle(
			const DirectX::XMVECTOR& spherePos,
			float radius,
			const DirectX::XMVECTOR trianglePos[3],
			IntersectionResult* result = {},
			bool triangleNormOnly = false);
		// ��VsAABB
		static bool IntersectSphereVsAABB(
			const DirectX::XMVECTOR& spherePos,
			float sphereRadius,
			const DirectX::XMVECTOR& aabbPos,
			const DirectX::XMVECTOR& aabbRadii,
			IntersectionResult* result);
		// ��vs�J�v�Z���̌�������
		static bool IntersectSphereVsCapsule(
			const DirectX::XMVECTOR& spherePos,
			float sphereRadius,
			const DirectX::XMVECTOR& capsulePos,
			const DirectX::XMVECTOR& capsuleDirection,
			float capsuleLength,
			float capsuleRadius,					// �J�v�Z���̔��a
			IntersectionResult* result = {});

		// AABB�ƕ��ʂ̍ŒZ�������擾����	���Ԃ�l���}�C�i�X���ƌ������Ă���󋵂ƒ�`
		static float GetMinDist_AABBPlane(
			const DirectX::XMVECTOR& aabbPos,
			const DirectX::XMVECTOR& aabbRadii,
			const DirectX::XMVECTOR& planeNorm,	// �v���K��
			const float planeDist,
			bool* reverse);				// �@���ɑ΂��ė����\���𔻒f����悤�̃t���O

		// AABBvs�O�p�`�̌�������
		static bool IntersectAABBVsTriangle(
			const DirectX::XMVECTOR& aabbPos,
			const DirectX::XMVECTOR& aabbRadii,
			const DirectX::XMVECTOR trianglePos[3],
			IntersectionResult* result);
		// AABBvsAABB
		static bool IntersectAABBVsAABB(
			const DirectX::XMVECTOR& aabbPos1,
			const DirectX::XMVECTOR& aabbRadii1,
			const DirectX::XMVECTOR& aabbPos2,
			const DirectX::XMVECTOR& aabbRadii2,
			IntersectionResult* result);

		// �����Ɛ����̍ŒZ�����̓����擾����
		static float GetMinDistSq_SegmentSegment(
			const  DirectX::XMVECTOR& point1A,
			const  DirectX::XMVECTOR& point1B,
			const  DirectX::XMVECTOR& point2A,
			const  DirectX::XMVECTOR& point2B,
			DirectX::XMVECTOR* nearPoint1,
			DirectX::XMVECTOR* nearPoint2);

		// �����ƎO�p�`�̍ŒZ�����̓����擾����
		static float GetMinDistSq_SegmentTriangle(
			const  DirectX::XMVECTOR& pointA,
			const  DirectX::XMVECTOR& pointB,
			const  DirectX::XMVECTOR trianglePos[3],
			DirectX::XMVECTOR* nearPointSegment,
			DirectX::XMVECTOR* nearPointTriangle,
			bool& nearPointEdghFlg);

		// �J�v�Z��vs�O�p�`
		static bool IntersectCapsuleVsTriangle(
			const DirectX::XMVECTOR& position,
			const DirectX::XMVECTOR& direction,
			float radius,
			float length,
			const DirectX::XMVECTOR trianglePos[3],
			IntersectionResult* result);

		// �J�v�Z��vs�J�v�Z���̌�������
		static bool IntersectCapsuleVsCapsule(
			const DirectX::XMVECTOR& position1,		// ���S
			const DirectX::XMVECTOR& direction1,	// �����i���K���j
			const float					length1,	// ����
			const float					radius1,	// ���a
			const DirectX::XMVECTOR& position2,		// ���S
			const DirectX::XMVECTOR& direction2,	// �����i���K���j
			const float					length2,	// ����
			const float					radius2,	// ���a
			IntersectionResult* result);

		// ���CVs�X���u3D (�����s�X���u)
		static bool IntersectRayVsSlub3D(
			const DirectX::XMVECTOR& rayStart,
			const DirectX::XMVECTOR& rayDirection,		// �v���K��
			float rayDist,
			const DirectX::XMVECTOR& slubCenter,
			const DirectX::XMVECTOR& slubRadii,
			HitResultVector* resultNear = {},			// ���C�ƃX���u�̍ŏ��̌�_���
			HitResultVector* resultFar = {});			// ���C�ƃX���u�̍Ō�̌�_���

		// ���CVsAABB
		static bool IntersectRayVsAABB(
			const DirectX::XMVECTOR& rayStart,
			const DirectX::XMVECTOR& rayDirection,		// �v���K��
			float rayDist,
			const DirectX::XMVECTOR& aabbPos,
			const DirectX::XMVECTOR& aabbRadii,
			HitResultVector* resultNear,
			HitResultVector* resultFar);

		// �O���̓_�ɑ΂���AABB�����̍ŋߓ_���擾����
		static DirectX::XMVECTOR GetClosestPoint_PointAABB(
			const DirectX::XMVECTOR& point,
			const DirectX::XMVECTOR& aabbPos,
			const DirectX::XMVECTOR& aabbRadii,
			bool surfaceFlg = false);

		// ������AABB�̍ŒZ�����̓����擾����
		static float GetMinDistSq_SegmentAABB(
			const DirectX::XMVECTOR& pointA,
			const DirectX::XMVECTOR& pointB,
			const DirectX::XMVECTOR& aabbPos,
			const DirectX::XMVECTOR& aabbRadii,
			DirectX::XMVECTOR* nearPointSegment = {},
			DirectX::XMVECTOR* nearPointAABB = {});

		// �J�v�Z��vsAABB�̌�������				���J�v�Z�����S�Ԃ̃��C��AABB�ƌ������Ă���ꍇ�̉����߂��̂��߂�result�̓��C�����ɓ����d�l�B���C�ȊO�̍ŒZ�����ł͂Ȃ��B
		static bool IntersectCapsuleVsAABB(
			const DirectX::XMVECTOR& position,	// ���S
			const DirectX::XMVECTOR& direction,	// �����i���K���j
			const float				length,	// ����
			const float				radius,	// ���a
			const DirectX::XMVECTOR& aabbPos,
			const DirectX::XMVECTOR& aabbRadii,
			IntersectionResult* result = {});

		// ���CVs��
		static bool IntersectRayVsSphere(
			const DirectX::XMVECTOR& rayStart,
			const DirectX::XMVECTOR& rayDirection,		// �v���K��
			float rayDist,
			const DirectX::XMVECTOR& spherePos,
			float radius,
			HitResultVector* result = {});

		// ���CVs�~��
		static bool IntersectRayVsOrientedCylinder(
			const DirectX::XMVECTOR& rayStart,
			const DirectX::XMVECTOR& rayDirection,		// �v���K��
			float rayDist,
			const DirectX::XMVECTOR& startCylinder,
			const DirectX::XMVECTOR& endCylinder,
			float radius,
			HitResultVector* result = {},
			DirectX::XMVECTOR* onCenterLinPos = {});

		// �X�t�B�A�L���X�gVs�O�p�`
		static bool IntersectSphereCastVsTriangle(
			const DirectX::XMVECTOR& sphereCastStart,		// �X�t�B�A�L���X�g�̔��˓_�i���̒��S�j
			const DirectX::XMVECTOR& sphereCastDirection,	// �X�t�B�A�L���X�g�̌����B�v���K��
			float sphereCastDist,
			float sphereCastRadius,							// �X�t�B�A�L���X�g�̔��a
			const DirectX::XMVECTOR trianglePos[3],
			HitResult* result = {},
			bool firstSphereChk = false);				// �X�t�B�A�L���X�g�̔��˓_�̋������ɎO�p�`�ɐڂ��Ă���ꍇ��false��Ԃ��`�F�b�N���s�����ǂ���
	};
}



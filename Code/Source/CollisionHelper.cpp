#include "CollisionHelper.h"
#include <algorithm>
#include <vector>

using namespace AbyssEngine;

// ��Vs��
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMVECTOR& position1,
	float radius1,
	const DirectX::XMVECTOR& position2,
	float radius2,
	IntersectionResult* result)
{
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(position1, position2);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// ��������
	float range = radius1 + radius2;
	if (lengthSq > range * range)
	{
		return false;
	}

	if (result)
	{
		result->normal = DirectX::XMVector3Normalize(vec);
		result->penetration = range - sqrtf(lengthSq);
		result->pointA = DirectX::XMVectorAdd(position1, DirectX::XMVectorScale(result->normal, -radius1));
		result->pointB = DirectX::XMVectorAdd(position2, DirectX::XMVectorScale(result->normal, radius2));
	}

	return true;
}

bool Collision::IntersectRayVsTriangle(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// �v���K��
	float rayDist,
	const DirectX::XMVECTOR triangleVerts[3],
	HitResultVector& result)
{
	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(triangleVerts[1], triangleVerts[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(triangleVerts[2], triangleVerts[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStart, DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, rayDist)));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

	if (d > 0.0f)	// �\������������Ă���Ƃ��̂ݔ�����s��
	{
		if (fabs(d) > 1e-6f)	// ���s�m�F
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStart, triangleVerts[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// ���C�̌����ƒ����m�F
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						result.distance = rayDist * t / d;
						result.position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, result.distance));
						result.normal = DirectX::XMVector3Normalize(norm);
						for (int i = 0; i < 3; i++)
						{
							result.triangleVerts[i] = triangleVerts[i];
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Collision::IntersectRayVsTriangle(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// �񐳋K��
	const DirectX::XMVECTOR triangleVerts[3],
	HitResultVector& result)
{
	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(triangleVerts[1], triangleVerts[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(triangleVerts[2], triangleVerts[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStart, DirectX::XMVectorAdd(rayStart, rayDirection));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

	if (d > 0.0f)	// �\������������Ă���Ƃ��̂ݔ�����s��
	{
		if (fabs(d) > 1e-6f)	// ���s�m�F
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStart, triangleVerts[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// ���C�̌����ƒ����m�F
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						DirectX::XMVECTOR tmp = DirectX::XMVectorScale(rayDirection, t / d);
						result.distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(tmp));
						result.position = DirectX::XMVectorAdd(rayStart, tmp);
						result.normal = DirectX::XMVector3Normalize(norm);
						for (int i = 0; i < 3; i++)
						{
							result.triangleVerts[i] = triangleVerts[i];
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Collision::IntersectRayVsTriangle(
	const DirectX::XMFLOAT3& rayStart,
	const DirectX::XMFLOAT3& rayDirection,		// �v���K��
	float rayDist,
	const DirectX::XMFLOAT3 triangleVerts[3],
	HitResult& result)
{
	DirectX::XMVECTOR rayStartVec = DirectX::XMLoadFloat3(&rayStart);
	DirectX::XMVECTOR rayDirectionVec = DirectX::XMLoadFloat3(&rayDirection);
	DirectX::XMVECTOR trianglePos[3] = {};
	for (int i = 0; i < 3; i++)
	{
		trianglePos[i] = DirectX::XMLoadFloat3(&triangleVerts[i]);
	}

	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStartVec, DirectX::XMVectorAdd(rayStartVec, DirectX::XMVectorScale(rayDirectionVec, rayDist)));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

	if (d > 0.0f)	// �\������������Ă���Ƃ��̂ݔ�����s��
	{
		if (fabs(d) > 1e-6f)	// ���s�m�F
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStartVec, trianglePos[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// ���C�̌����ƒ����m�F
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						result.distance = rayDist * t / d;
						DirectX::XMStoreFloat3(&result.position, DirectX::XMVectorAdd(rayStartVec, DirectX::XMVectorScale(rayDirectionVec, result.distance)));
						DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(norm));
						for (int i = 0; i < 3; i++)
						{
							result.triangleVerts[i] = triangleVerts[i];
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

// �O���̓_�ɑ΂���O�p�`�����̍ŋߓ_���Z�o����
// �Ԃ�l�FnearPos���O�p�`�����i�ӂⒸ�_�łȂ��j�̎�true
bool Collision::GetClosestPoint_PointTriangle(
	const DirectX::XMVECTOR& point,				// ��r���W
	const DirectX::XMVECTOR trianglePos[3],		// �O�p�`�̒��_	
	DirectX::XMVECTOR& nearPos)					// �O�p�`���̍ŋߓ_
{
	// point��TrianglePos[0]�̊O���̒��_�̈�ɂ��邩�`�F�b�N
	DirectX::XMVECTOR Vec01;
	DirectX::XMVECTOR Vec02;
	DirectX::XMVECTOR Vec0P;
	float d1;
	float d2;



	// point��TrianglePos[1]�̊O���̒��_�̈�ɂ��邩�`�F�b�N
	DirectX::XMVECTOR Vec1P;
	float d3;
	float d4;



	// point��TrianglePos[0]��TrianglePos[1]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
	float v;



	// point��TrianglePos[2]�̊O���̒��_�̈�ɂ��邩�`�F�b�N
	DirectX::XMVECTOR Vec2P;
	float d5;
	float d6;



	// point��TrianglePos[0]��TrianglePos[2]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
	float u;



	// point��TrianglePos[1]��TrianglePos[2]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
	float w;



	// �����܂ł���΁AnearPos�͎O�p�`�̓����ɂ���
	float t01;
	float t02;




	return true;
}

// ��Vs�O�p�`
bool Collision::IntersectSphereVsTriangle(
	const DirectX::XMVECTOR& spherePos,
	float radius,
	const DirectX::XMVECTOR trianglePos[3],
	IntersectionResult* result,
	bool triangleNormOnly)
{
	DirectX::XMVECTOR tmpPos = {};
	GetClosestPoint_PointTriangle(spherePos, trianglePos, tmpPos);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(spherePos, tmpPos);
	bool ret = false;
	if (result)
	{
		DirectX::XMVECTOR vecNorm = DirectX::XMVector3Normalize(vec);
		result->pointA = DirectX::XMVectorAdd(spherePos, DirectX::XMVectorScale(vecNorm, radius));
		result->pointB = tmpPos;
		ret = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec)) <= (radius * radius);

		if (ret && triangleNormOnly)
		{
			result->normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]), DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0])));
			float dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(result->normal, vec));
			result->penetration = radius - dist;
		}
		else
		{
			result->normal = vecNorm;
			result->penetration = radius - DirectX::XMVectorGetX(DirectX::XMVector3Length(vec));
		}
	}
	return ret;
}

// ��vsAABB�̌�������
bool Collision::IntersectSphereVsAABB(
	const DirectX::XMVECTOR& spherePos,
	float sphereRadius,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	IntersectionResult* result)
{
	DirectX::XMFLOAT3 aabbPosF3 = {};
	DirectX::XMStoreFloat3(&aabbPosF3, aabbPos);
	DirectX::XMFLOAT3 aabbRadiiF3 = {};
	DirectX::XMStoreFloat3(&aabbRadiiF3, aabbRadii);
	DirectX::XMFLOAT3 hitPos = {};
	DirectX::XMStoreFloat3(&hitPos, spherePos);

	if (hitPos.x < aabbPosF3.x - aabbRadiiF3.x)
	{
		hitPos.x = aabbPosF3.x - aabbRadiiF3.x;
	}
	else if (hitPos.x > aabbPosF3.x + aabbRadiiF3.x)
	{
		hitPos.x = aabbPosF3.x + aabbRadiiF3.x;
	}

	if (hitPos.y < aabbPosF3.y - aabbRadiiF3.y)
	{
		hitPos.y = aabbPosF3.y - aabbRadiiF3.y;
	}
	else if (hitPos.y > aabbPosF3.y + aabbRadiiF3.y)
	{
		hitPos.y = aabbPosF3.y + aabbRadiiF3.y;
	}

	if (hitPos.z < aabbPosF3.z - aabbRadiiF3.z)
	{
		hitPos.z = aabbPosF3.z - aabbRadiiF3.z;
	}
	else if (hitPos.z > aabbPosF3.z + aabbRadiiF3.z)
	{
		hitPos.z = aabbPosF3.z + aabbRadiiF3.z;
	}

	// ��������
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(spherePos, DirectX::XMLoadFloat3(&hitPos));
	float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

	if (result)
	{
		result->penetration = sphereRadius - sqrtf(lengthSq);
		result->pointB = DirectX::XMLoadFloat3(&hitPos);
		result->normal = DirectX::XMVector3Normalize(Vec);
		result->pointA = DirectX::XMVectorAdd(spherePos, DirectX::XMVectorScale(result->normal, -sphereRadius));
	}

	return (lengthSq < sphereRadius * sphereRadius);
}

// ��vs�J�v�Z���̌�������
bool Collision::IntersectSphereVsCapsule(
	const DirectX::XMVECTOR& spherePos,
	float sphereRadius,
	const DirectX::XMVECTOR& capsulePos,
	const DirectX::XMVECTOR& capsuleDirection,
	float capsuleLength,
	float capsuleRadius,					// �J�v�Z���̔��a
	IntersectionResult* result)
{
	float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(capsuleDirection, DirectX::XMVectorSubtract(spherePos, capsulePos)));	// �ˉe���̎Z�o
	DirectX::XMVECTOR point = {};	// �J�v�Z�����S����̍ŋߓ_
	if (t < 0)
	{
		point = capsulePos;
	}
	else if (t > capsuleLength)
	{
		point = DirectX::XMVectorAdd(capsulePos, DirectX::XMVectorScale(capsuleDirection, capsuleLength));
	}
	else
	{
		point = DirectX::XMVectorAdd(capsulePos, DirectX::XMVectorScale(capsuleDirection, t));
	}

	// ��������
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(spherePos, point);
	float radiusAdd = sphereRadius + capsuleRadius;
	float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));

	if (result)
	{
		result->normal = DirectX::XMVector3Normalize(vec);
		result->penetration = radiusAdd - sqrtf(distSq);
		result->pointA = DirectX::XMVectorAdd(spherePos, DirectX::XMVectorScale(result->normal, -sphereRadius));
		result->pointB = DirectX::XMVectorAdd(point, DirectX::XMVectorScale(result->normal, capsuleRadius));
	}

	return distSq < radiusAdd * radiusAdd;
}

// ���CVs�X���u3D (�����s�X���u)
bool Collision::IntersectRayVsSlub3D(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// �v���K��
	float rayDist,
	const DirectX::XMVECTOR& slubCenter,
	const DirectX::XMVECTOR& slubRadii,
	HitResultVector* resultNear,		// ���C�ƃX���u�̍ŏ��̌�_���
	HitResultVector* resultFar)		// ���C�ƃX���u�̍Ō�̌�_���
{
	// ���[�v�������邽�߁Axyz������z��ɑ������
	float startPosArray[3] = { DirectX::XMVectorGetX(rayStart), DirectX::XMVectorGetY(rayStart), DirectX::XMVectorGetZ(rayStart) };
	float slubCenterArray[3] = { DirectX::XMVectorGetX(slubCenter), DirectX::XMVectorGetY(slubCenter), DirectX::XMVectorGetZ(slubCenter) };
	float slubRadiiArray[3] = { DirectX::XMVectorGetX(slubRadii), DirectX::XMVectorGetY(slubRadii), DirectX::XMVectorGetZ(slubRadii) };
	float dArray[3] = { DirectX::XMVectorGetX(rayDirection), DirectX::XMVectorGetY(rayDirection), DirectX::XMVectorGetZ(rayDirection) };

	// �����ƃX���u�̂Q��_�܂ł̋�����tmin��tmax�ƒ�`
	float tmin = 0.0f;
	float tmax = FLT_MAX;
	int minAxis = 0;

	bool ret = true;

	// �X���u�Ƃ̋������Z�o���������Ă��邩�̊m�F�ƍŋߓ_�̎Z�o���s��
	for (int i = 0; i < 3; i++)
	{
		//xyz���Ƃ̕��s�m�F
		if (fabsf(dArray[i]) < FLT_EPSILON)
		{
			// ���s�̏ꍇ�A�ʒu�֌W�̔�r���s���͈͓��ɂȂ���Ό����Ȃ�
			if (startPosArray[i] < slubCenterArray[i] - slubRadiiArray[i] || startPosArray[i] > slubCenterArray[i] + slubRadiiArray[i])
			{
				ret = false;
			}
		}
		else
		{
			// t1���߃X���u�At2�����X���u�Ƃ̋���
			float ood = 1.0f / dArray[i];
			float t1 = (slubCenterArray[i] - slubRadiiArray[i] - startPosArray[i]) * ood;
			float t2 = (slubCenterArray[i] + slubRadiiArray[i] - startPosArray[i]) * ood;

			// ���߂��t�]���Ă���ꍇ������̂ŁA���̏ꍇ����ւ��Ă���
			if (t1 > t2)
			{
				float tmp = t1;
				t1 = t2;
				t2 = tmp;
			}

			// t1��tmin�����傫���ꍇ�Atmin��t1�ōX�V����
			if (t1 > tmin)
			{
				tmin = t1;
				minAxis = i;
			}

			// t2��tmax�����������ꍇ�Atmax��t2�ōX�V����
			if (t2 < tmax)
			{
				tmax = t2;
			}
		}
	}

	if (resultNear)
	{
		resultNear->distance = tmin;
		resultNear->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, resultNear->distance));
		DirectX::XMVECTOR norm = { static_cast<float>(minAxis == 0),   static_cast<float>(minAxis == 1),  static_cast<float>(minAxis == 2) };
		if (dArray[minAxis] > 0)
		{
			norm = DirectX::XMVectorNegate(norm);
		}
		resultNear->normal = norm;

		if (resultFar)
		{
			resultFar->distance = tmax;
			resultFar->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, resultFar->distance));
			DirectX::XMVECTOR norm = { static_cast<float>(minAxis == 0),   static_cast<float>(minAxis == 1),  static_cast<float>(minAxis == 2) };
			if (dArray[minAxis] > 0)
			{
				norm = DirectX::XMVectorNegate(norm);
			}
			resultFar->normal = norm;
		}
	}

	return ret && (rayDist >= tmin);
}

// ���CVsAABB
bool Collision::IntersectRayVsAABB(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// �v���K��
	float rayDist,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	HitResultVector* resultNear,
	HitResultVector* resultFar)
{
	HitResultVector nearPoint = {};
	HitResultVector farPoint = {};
	if (IntersectRayVsSlub3D(rayStart, rayDirection, rayDist, aabbPos, aabbRadii, &nearPoint, &farPoint))
	{
		if (nearPoint.distance < farPoint.distance)
		{
			if (resultNear)
			{
				*resultNear = nearPoint;
				if (resultFar)
				{
					*resultFar = farPoint;
				}
			}
			return true;
		}
	}

	return false;
}

// �O���̓_�ɑ΂���AABB�����̍ŋߓ_���擾����
DirectX::XMVECTOR Collision::GetClosestPoint_PointAABB(
	const DirectX::XMVECTOR& point,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	bool surfaceFlg)
{
	float aabbPosArray[3] = { DirectX::XMVectorGetX(aabbPos), DirectX::XMVectorGetY(aabbPos), DirectX::XMVectorGetZ(aabbPos) };
	float aabbRadArray[3] = { DirectX::XMVectorGetX(aabbRadii), DirectX::XMVectorGetY(aabbRadii), DirectX::XMVectorGetZ(aabbRadii) };
	float nearPoint[3] = { DirectX::XMVectorGetX(point), DirectX::XMVectorGetY(point), DirectX::XMVectorGetZ(point) };

	int targetAxis = -1;
	int count = 0;
	float dist = FLT_MAX;

	for (int i = 0; i < 3; i++)
	{
		if (nearPoint[i] < aabbPosArray[i] - aabbRadArray[i])
		{
			nearPoint[i] = aabbPosArray[i] - aabbRadArray[i];
		}
		else if (nearPoint[i] > aabbPosArray[i] + aabbRadArray[i])
		{
			nearPoint[i] = aabbPosArray[i] + aabbRadArray[i];
		}
		else if (nearPoint[i] <= aabbPosArray[i])
		{
			if (fabsf(dist) > fabsf(aabbPosArray[i] - aabbRadArray[i] - nearPoint[i]))
			{
				targetAxis = i;
				dist = aabbPosArray[i] - aabbRadArray[i] - nearPoint[i];
			}
			count++;
		}
		else if (nearPoint[i] >= aabbPosArray[i])
		{
			if (fabsf(dist) > fabsf(aabbPosArray[i] + aabbRadArray[i] - nearPoint[i]))
			{
				targetAxis = i;
				dist = aabbPosArray[i] + aabbRadArray[i] - nearPoint[i];
			}
			count++;
		}
	}

	// surfaceFlg��true����count��3�̏ꍇ�A�����ɂ��邽�߁A�ŋߓ_��\�ʂɕ␳����
	if (surfaceFlg)
	{
		if (count == 3)
		{
			if (nearPoint[targetAxis] < aabbPosArray[targetAxis])
			{
				nearPoint[targetAxis] = aabbPosArray[targetAxis] - aabbRadArray[targetAxis];
			}
			else if (nearPoint[targetAxis] > aabbPosArray[targetAxis])
			{
				nearPoint[targetAxis] = aabbPosArray[targetAxis] + aabbRadArray[targetAxis];
			}
		}
	}

	return DirectX::XMVECTOR{ nearPoint[0], nearPoint[1], nearPoint[2] };
}

// �X�t�B�A�L���X�gor�J�v�Z��VsAABB�p�̒��_�Z�o�֐�
inline DirectX::XMVECTOR GetAABBCorner(
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const int v)
{
	DirectX::XMFLOAT3 p = {};
	DirectX::XMStoreFloat3(&p, aabbPos);
	p.x += (v & (1 << 0)) ? DirectX::XMVectorGetX(aabbRadii) : -DirectX::XMVectorGetX(aabbRadii);
	p.y += (v & (1 << 1)) ? DirectX::XMVectorGetY(aabbRadii) : -DirectX::XMVectorGetY(aabbRadii);
	p.z += (v & (1 << 2)) ? DirectX::XMVectorGetZ(aabbRadii) : -DirectX::XMVectorGetZ(aabbRadii);

	return DirectX::XMLoadFloat3(&p);
}

// ������AABB�̍ŒZ�����ɂ��āA�ʗ̈�ɐ����̒[�_������ꍇ�̏���
inline float GetMinDistSq_SegmentAABBFace_PointInFaceArea(
	const  DirectX::XMVECTOR& targetPoint,
	const  DirectX::XMVECTOR& otherPoint,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const int u,
	const int v,
	DirectX::XMVECTOR* nearPointSegment,
	DirectX::XMVECTOR* nearPointAABB)
{
	DirectX::XMVECTOR nearTargetPoint = Collision::GetClosestPoint_PointAABB(targetPoint, aabbPos, aabbRadii);
	float minDist = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(targetPoint, nearTargetPoint)));
	if (nearPointSegment) *nearPointSegment = targetPoint;
	if (nearPointSegment) *nearPointAABB = nearTargetPoint;

	std::vector<DirectX::XMVECTOR> aabbFaceCorner;
	if (u == 1)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
	}
	else if (u == 2)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
	}
	else if (u == 4)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}
	else if (v == 1)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
	}
	else if (v == 2)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
	}
	else if (v == 4)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
	}


	DirectX::XMVECTOR tmpNearPointSegment = {};
	DirectX::XMVECTOR tmpNearPointAABB = {};
	float dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[0], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}

	return minDist;
}

// ������AABB�̍ŒZ�����ɂ��āA���_�̈�ɐ����̒[�_������ꍇ�̏���
inline float GetMinDistSq_SegmentAABBFace_PointInCornerArea(
	const  DirectX::XMVECTOR& targetPoint,
	const  DirectX::XMVECTOR& otherPoint,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const int u,
	const int v,
	DirectX::XMVECTOR* nearPointSegment,
	DirectX::XMVECTOR* nearPointAABB)
{
	std::vector<DirectX::XMVECTOR> aabbFaceCorner;
	if (u == 7 && v == 0)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
	}
	else if (u == 6 && v == 1)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}
	else if (u == 5 && v == 2)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
	}
	else if (u == 4 && v == 3)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
	}
	else if (u == 3 && v == 4)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
	}
	else if (u == 2 && v == 5)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
	}
	else if (u == 1 && v == 6)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
	}
	else if (u == 0 && v == 7)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
	}

	DirectX::XMVECTOR tmpNearPointSegment = {};
	DirectX::XMVECTOR tmpNearPointAABB = {};
	float minDist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], nearPointSegment, nearPointAABB);
	float dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[0], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[6], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[4], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[5], aabbFaceCorner[6], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}

	return minDist;
}

// ������AABB�̍ŒZ�����ɂ��āA�ӗ̈�ɐ����̒[�_������ꍇ�̏���
inline float GetMinDistSq_SegmentAABBFace_PointInEdgeArea(
	const  DirectX::XMVECTOR& targetPoint,
	const  DirectX::XMVECTOR& otherPoint,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const int u,
	const int v,
	DirectX::XMVECTOR* nearPointSegment,
	DirectX::XMVECTOR* nearPointAABB)
{
	std::vector<DirectX::XMVECTOR> aabbFaceCorner;
	if (u == 1)
	{
		if (v == 2)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		}
		else if (v == 4)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		}
	}
	else if (u == 2)
	{
		if (v == 1)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		}
		else if (v == 4)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		}
	}
	else if (u == 3)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
	}
	else if (u == 4)
	{
		if (v == 1)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		}
		else if (v == 2)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		}
	}
	else if (u == 5)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
	}
	else if (u == 6)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}
	else if (v == 3)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}
	else if (v == 5)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
	}
	else if (v == 6)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}


	DirectX::XMVECTOR tmpNearPointSegment = {};
	DirectX::XMVECTOR tmpNearPointAABB = {};
	float minDist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], nearPointSegment, nearPointAABB);
	float dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[4], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}

	return minDist;
}

// ������AABB�̍ŒZ�����̓����擾����
float Collision::GetMinDistSq_SegmentAABB(
	const  DirectX::XMVECTOR& pointA,
	const  DirectX::XMVECTOR& pointB,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	DirectX::XMVECTOR* nearPointSegment,
	DirectX::XMVECTOR* nearPointAABB)
{
	// ���������C�ƌ����Ă�AABB�ƌ������邩�m�F
	DirectX::XMVECTOR rayDirection = DirectX::XMVectorSubtract(pointB, pointA);
	float rayDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(rayDirection));
	rayDirection = DirectX::XMVector3Normalize(rayDirection);
	HitResultVector tmpResultNear = {}, tmpResultFar = {};
	float minDistanceSq = 0.0f;

	if (IntersectRayVsAABB(pointA, rayDirection, rayDist, aabbPos, aabbRadii, &tmpResultNear, &tmpResultFar))
	{
		if (nearPointSegment)
		{
			if (tmpResultNear.distance == 0.0f)
			{
				*nearPointSegment = pointA;
			}
			else
			{
				*nearPointSegment = pointB;
			}
		}
		if (nearPointAABB)
		{
			if (tmpResultNear.distance == 0.0f)
			{
				*nearPointAABB = tmpResultFar.position;
			}
			else
			{
				*nearPointAABB = tmpResultNear.position;
			}
		}

		return 0.0f;
	}
	else
	{
		int uA = 0, vA = 0;
		if (DirectX::XMVectorGetX(pointA) <= DirectX::XMVectorGetX(aabbPos) - DirectX::XMVectorGetX(aabbRadii))			uA |= (1 << 0);
		else if (DirectX::XMVectorGetX(pointA) >= DirectX::XMVectorGetX(aabbPos) + DirectX::XMVectorGetX(aabbRadii))	vA |= (1 << 0);
		if (DirectX::XMVectorGetY(pointA) <= DirectX::XMVectorGetY(aabbPos) - DirectX::XMVectorGetY(aabbRadii))			uA |= (1 << 1);
		else if (DirectX::XMVectorGetY(pointA) >= DirectX::XMVectorGetY(aabbPos) + DirectX::XMVectorGetY(aabbRadii))	vA |= (1 << 1);
		if (DirectX::XMVectorGetZ(pointA) <= DirectX::XMVectorGetZ(aabbPos) - DirectX::XMVectorGetZ(aabbRadii))			uA |= (1 << 2);
		else if (DirectX::XMVectorGetZ(pointA) >= DirectX::XMVectorGetZ(aabbPos) + DirectX::XMVectorGetZ(aabbRadii))	vA |= (1 << 2);
		int maskA = uA | vA;

		int uB = 0, vB = 0;
		if (DirectX::XMVectorGetX(pointB) <= DirectX::XMVectorGetX(aabbPos) - DirectX::XMVectorGetX(aabbRadii))			uB |= (1 << 0);
		else if (DirectX::XMVectorGetX(pointB) >= DirectX::XMVectorGetX(aabbPos) + DirectX::XMVectorGetX(aabbRadii))	vB |= (1 << 0);
		if (DirectX::XMVectorGetY(pointB) <= DirectX::XMVectorGetY(aabbPos) - DirectX::XMVectorGetY(aabbRadii))			uB |= (1 << 1);
		else if (DirectX::XMVectorGetY(pointB) >= DirectX::XMVectorGetY(aabbPos) + DirectX::XMVectorGetY(aabbRadii))	vB |= (1 << 1);
		if (DirectX::XMVectorGetZ(pointB) <= DirectX::XMVectorGetZ(aabbPos) - DirectX::XMVectorGetZ(aabbRadii))			uB |= (1 << 2);
		else if (DirectX::XMVectorGetZ(pointB) >= DirectX::XMVectorGetZ(aabbPos) + DirectX::XMVectorGetZ(aabbRadii))	vB |= (1 << 2);
		int maskB = uB | vB;

		// �����̈�̏ꍇ
		if (uA == uB && vA == vB)
		{
			DirectX::XMVECTOR nearPointA = GetClosestPoint_PointAABB(pointA, aabbPos, aabbRadii);
			DirectX::XMVECTOR nearPointB = GetClosestPoint_PointAABB(pointB, aabbPos, aabbRadii);
			float nearPointDistA = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(pointA, nearPointA)));
			float nearPointDistB = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(pointB, nearPointB)));

			DirectX::XMVECTOR cornerA = GetAABBCorner(aabbPos, aabbRadii, vA);
			DirectX::XMVECTOR cornerB = GetAABBCorner(aabbPos, aabbRadii, 7 - uA);
			float edgeDist = GetMinDistSq_SegmentSegment(pointA, pointB, cornerA, cornerB, nearPointSegment, nearPointAABB);

			float ret = 0.0f;
			if (nearPointDistA < nearPointDistB)
			{
				if (edgeDist < nearPointDistA)
				{
					ret = edgeDist;
				}
				else
				{
					ret = nearPointDistA;
					if (nearPointSegment) *nearPointSegment = pointA;
					if (nearPointAABB) *nearPointAABB = nearPointA;
				}
			}
			else
			{
				if (edgeDist < nearPointDistB)
				{
					ret = edgeDist;
				}
				else
				{
					ret = nearPointDistB;
					if (nearPointSegment) *nearPointSegment = pointB;
					if (nearPointAABB) *nearPointAABB = nearPointB;
				}
			}

			return ret;
		}
		else
		{
			// ���ꂼ��Ⴄ�ʗ̈�̏ꍇ
			if ((maskA & (maskA - 1)) == 0 && (maskB & (maskB - 1)) == 0)
			{
				int uAdd = uA + uB;
				int vAdd = vA + vB;
				DirectX::XMVECTOR cornerA = GetAABBCorner(aabbPos, aabbRadii, vAdd);
				DirectX::XMVECTOR cornerB = GetAABBCorner(aabbPos, aabbRadii, 7 - uAdd);

				return GetMinDistSq_SegmentSegment(pointA, pointB, cornerA, cornerB, nearPointSegment, nearPointAABB);
			}
			// pointA���݂̂��ʗ̈�̏ꍇ
			else if ((maskA & (maskA - 1)) == 0)
			{
				return GetMinDistSq_SegmentAABBFace_PointInFaceArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
			}
			// pointB���݂̂��ʗ̈�̏ꍇ
			else if ((maskB & (maskB - 1)) == 0)
			{
				return GetMinDistSq_SegmentAABBFace_PointInFaceArea(pointB, pointA, aabbPos, aabbRadii, uB, vB, nearPointSegment, nearPointAABB);
			}
			// ���������_�̈�̏ꍇ
			else if (maskA == 7 && maskB == 7)
			{
				return GetMinDistSq_SegmentAABBFace_PointInCornerArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
			}
			// pointA���݂̂����_�̈�̏ꍇ
			else if (maskA == 7)
			{
				return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointB, pointA, aabbPos, aabbRadii, uB, vB, nearPointSegment, nearPointAABB);
			}
			// pointB���݂̂����_�̈�̏ꍇ
			else if (maskB == 7)
			{
				return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
			}
			// �������ӗ̈�̏ꍇ
			else
			{
				return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
			}
		}
	}
}


// �J�v�Z��vsAABB�̌�������
bool Collision::IntersectCapsuleVsAABB(
	const DirectX::XMVECTOR& position,	// ���S
	const DirectX::XMVECTOR& direction,	// �����i���K���j
	const float				length,	// ����
	const float				radius,	// ���a
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	IntersectionResult* result)
{
	float dist = 0.0f;
	DirectX::XMVECTOR nearPointSegment = {};
	DirectX::XMVECTOR nearPointAABB = {};

	DirectX::XMVECTOR end = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, length));

	if (result)
	{
		dist = GetMinDistSq_SegmentAABB(position, end, aabbPos, aabbRadii, &nearPointSegment, &nearPointAABB);
	}
	else
	{
		dist = GetMinDistSq_SegmentAABB(position, end, aabbPos, aabbRadii);
	}

	if (dist < radius * radius)
	{
		if (result)
		{
			if (dist == 0.0f)
			{
				result->normal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(nearPointAABB, nearPointSegment));
				result->pointA = DirectX::XMVectorAdd(nearPointSegment, DirectX::XMVectorScale(result->normal, -radius));
				result->pointB = nearPointAABB;
				result->penetration = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(result->pointB, result->pointA)));
			}
			else
			{
				result->penetration = radius - sqrtf(dist);
				result->normal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(nearPointSegment, nearPointAABB));
				result->pointA = DirectX::XMVectorAdd(nearPointSegment, DirectX::XMVectorScale(result->normal, -radius));
				result->pointB = nearPointAABB;
			}
		}
		return true;
	}

	return false;
}


// ���CVs��
bool Collision::IntersectRayVsSphere(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// �v���K��
	float rayDist,
	const DirectX::XMVECTOR& spherePos,
	float radius,
	HitResultVector* result)
{
	DirectX::XMVECTOR ray2sphere = DirectX::XMVectorSubtract(spherePos, rayStart);
	float projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, rayDirection));
	float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;

	if (distSq < radius * radius)
	{
		float distance = projection - sqrtf(radius * radius - distSq);
		if (distance > 0.0f)
		{
			if (distance < rayDist)
			{
				if (result)
				{
					result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, distance));
					result->distance = distance;
					result->normal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(result->position, spherePos));
				}
				return true;
			}
		}
	}

	return false;
}

// ���CVs�~��
bool Collision::IntersectRayVsOrientedCylinder(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// �v���K��
	float rayDist,
	const DirectX::XMVECTOR& startCylinder,
	const DirectX::XMVECTOR& endCylinder,
	float radius,
	HitResultVector* result,
	DirectX::XMVECTOR* onCenterLinPos)
{
	DirectX::XMVECTOR d = DirectX::XMVectorSubtract(endCylinder, startCylinder);
	DirectX::XMVECTOR m = DirectX::XMVectorSubtract(rayStart, startCylinder);
	DirectX::XMVECTOR n = DirectX::XMVectorScale(rayDirection, rayDist);

	float md = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, d));
	float nd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, d));
	float dd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, d));

	// �����S�̂��~���̒�ʁE��ʂɐ����ȃX���u�ɑ΂��ĊO���ɂ��邩�ǂ����𔻒�

	//���C�̎n�_���X���uPQ�̊O���ɂ���
	if (md < 0)
	{
		//���C�̐�[���X���uPQ�ɓ͂��Ȃ���
		//float mdDot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, d));
		//float ndDot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, d));
		if (md + nd < 0)
		{
			//��ʂ��܂ޕ��ʂƌ������Ȃ�
			return false;
		}
	}
	//��ʂ̊O���ɂ��邩�ǂ���
	else if (md > 0)
	{
		if (md > dd && md + nd > dd)
		{
			return false;
		}
	}

	float nn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, n));
	float a = nn * dd - nd * nd;
	float mm = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, m));
	float k = mm - radius * radius;
	float c = mm * dd - md * md - radius * radius * dd;

	// �������~���̎��ɑ΂��ĕ��s
	if (fabsf(a) < 0.0001f)		// �덷���o�₷���v�Z�Ȃ̂�臒l�͑傫�߁i0.0001f�j
	{
		if (c > 0.0f) return false;	// �����͉~���̊O��

		if (result)
		{
			// ��ʌ�����result���Z�o
			if (md < 0)
			{
				float t = -md / nd;
				//�����_�Z�o
				result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(n, t));

				//�@���Z�o
				DirectX::XMVECTOR nLength = DirectX::XMVector3Length(n);
				result->normal = DirectX::XMVectorDivide(DirectX::XMVectorSubtract({}, n), nLength);

				//�����Z�o
				result->distance = t * DirectX::XMVectorGetX(nLength);
			}
			// ��ʌ�����result���Z�o
			else
			{
				float t = (dd - md) / nd;
				//�����_�Z�o
				result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(n, t));

				//�@���Z�o
				DirectX::XMVECTOR nLength = DirectX::XMVector3Length(n);
				result->normal = DirectX::XMVectorDivide(DirectX::XMVectorSubtract({}, n), nLength);

				//�����Z�o
				result->distance = t * DirectX::XMVectorGetX(nLength);
			}

		}
		return true;
	}

	// �������~���̎��ɑ΂��ĕ��s�łȂ�
	// �~���̕\�ʂ�\���A�֐��������ƒ����̕������̉������߂Č���������s���B
	float mn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, n));
	float b = mn * dd - nd * md;
	float D = b * b - a * c;	// ���ʎ�

	if (D < 0) return false;	// ���������Ȃ��̂Ō������Ă��Ȃ�

	// ���̌����ɂ��A��_�܂ł̋������Z�o
	float hitDistance = -b - sqrtf(D);
	if (hitDistance < 0.0f)
	{
		hitDistance = (-b + sqrtf(D));
		if (hitDistance < 0.0f)
		{
			return false;	// ��_�������̊O���ɂ���������Ă��Ȃ�
		}
	}
	else if (hitDistance > a)
	{
		hitDistance = (-b + sqrtf(D));
		if (hitDistance > a)
		{
			return false;	// ��_�������̊O���ɂ���������Ă��Ȃ�
		}
	}
	hitDistance /= a;

	// ���̌����̌��ʁA�~����startCylinder���̒�ʂ̊O�Ō������Ă��邩�m�F
	if (md + hitDistance * nd < 0.0f)
	{
		// ��ʂƂ̌������m�F���A�������Ă����hitDistance���X�V
		float t = -md / nd;
		float v = mm + 2 * t * mn + t * t * nn;
		if (v > radius * radius)
		{
			//�������Ȃ�
			return false;
		}
		hitDistance = t;
	}
	// ���̌����̌��ʁA�~����endCylinder���̏�ʂ̊O�Ō������Ă��邩�m�F
	else if (md + hitDistance * nd > dd)
	{
		// ��ʂƂ̌������m�F���A�������Ă����hitDistance���X�V
		float t = -(md - dd) / nd;
		float v = mm + 2 * t * mn - 2 * md + t * t * nn - 2 * t * nd + dd;
		if (v > radius * radius)
		{
			//�������Ȃ�
			return false;
		}
		hitDistance = t;
	}

	// �������~���̒�ʂƏ�ʂ̊ԂŌ������Ă��邱�Ƃ��m��i��ʁE��ʂł̌������܂ށj
	if (result)
	{
		// result�̊e�p�����[�^���Z�o����
		result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(n, hitDistance));

		//�@���Z�o
		DirectX::XMVECTOR nLength = DirectX::XMVector3Length(n);
		result->normal = DirectX::XMVectorDivide(DirectX::XMVectorSubtract({}, n), nLength);

		//�����Z�o
		result->distance = hitDistance * DirectX::XMVectorGetX(nLength);

		if (onCenterLinPos)
		{
			//�~���̎���ɂ�����ŋߓ_�̎Z�o
			DirectX::XMVECTOR DNormal = DirectX::XMVector3Normalize(d);
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(result->position, startCylinder);
			DirectX::XMVECTOR Proj = DirectX::XMVectorScale(DNormal, DirectX::XMVectorGetX(DirectX::XMVector3Dot(DNormal, V)));
			*onCenterLinPos = DirectX::XMVectorAdd(startCylinder, Proj);
		}
	}

	return true;
}

// �X�t�B�A�L���X�gVs�O�p�`
bool Collision::IntersectSphereCastVsTriangle(
	const DirectX::XMVECTOR& sphereCastStart,
	const DirectX::XMVECTOR& sphereCastDirection,
	float sphereCastDist,
	float sphereCastRadius,
	const DirectX::XMVECTOR trianglePos[3],
	HitResult* result,
	bool firstSphereChk)
{
	// �O�p�`�ƃX�^�[�g�ʒu�ɂ����鋅���������Ă���ꍇ�́Afalse�ŏI������
	if (firstSphereChk)
	{
		if (IntersectSphereVsTriangle(sphereCastStart, sphereCastRadius, trianglePos))
		{
			return false;
		}
	}

	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR inverceDirection = DirectX::XMVectorScale(sphereCastDirection, -sphereCastDist);
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, inverceDirection));
	bool hitFlg = false;

	if (d >= 0.0f)	// �\������������Ă���Ƃ��̂ݔ�����s��
	{
		// �O�p�`�̊e���_��@�������ɋ����a�����ړ�������
		DirectX::XMVECTOR fixVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(norm), sphereCastRadius);

		// �ړ���̎O�p�`�ƃX�t�B�A�L���X�g�̒��S���C����������Ȃ�A���̎O�p�`�̓���(�ʗ̈�)�ŃX�t�B�A�L���X�g���������邱�Ƃ��m��
		DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(sphereCastStart, DirectX::XMVectorAdd(trianglePos[0], fixVec));
		float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));

		if (t >= 0.0f && t < d)		// ���C�̌����ƒ����m�F
		{
			DirectX::XMVECTOR cross = DirectX::XMVector3Cross(inverceDirection, ap);
			float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, cross));
			if (v >= 0.0f && v <= d)
			{
				float w = -1.0f * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, cross));
				if (w > 0.0f && v + w <= d)
				{
					if (result)
					{
						DirectX::XMVECTOR crossPos = DirectX::XMVectorAdd(sphereCastStart, DirectX::XMVectorScale(inverceDirection, -t / d));
						result->distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(crossPos, sphereCastStart)));
						DirectX::XMStoreFloat3(&result->position, DirectX::XMVectorSubtract(crossPos, fixVec));
						DirectX::XMStoreFloat3(&result->normal, DirectX::XMVector3Normalize(norm));
						DirectX::XMStoreFloat3(&result->triangleVerts[0], trianglePos[0]);
						DirectX::XMStoreFloat3(&result->triangleVerts[1], trianglePos[1]);
						DirectX::XMStoreFloat3(&result->triangleVerts[2], trianglePos[2]);
					}
					return true;
				}
			}
		}

		// �ʗ̈�Ō������Ȃ���΁A�{���m�C�̊e���_�̈�A�ӗ̈�Ō���������s���A�ŒZ�������Z�o����
		enum class IntersectPattern
		{
			enNone = -1,
			enVertex0 = 1,
			enVertex1,
			enVertex2,
			enEdge01,
			enEdge02,
			enEdge12
		};
		HitResultVector tmpResult = {};
		DirectX::XMVECTOR minPosition = {}, minNormal = {}, tmpOnCenterLinePos = {}, minOnCenterLinePos = {};
		float minDistance = sphereCastDist;
		IntersectPattern minDistCalcPattern = IntersectPattern::enNone;

		// trianglePos[0] ���_�̈�̃`�F�b�N
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex0;
				hitFlg = true;
			}
		}

		// trianglePos[1] ���_�̈�̃`�F�b�N
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[1], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex1;
				hitFlg = true;
			}
		}

		// trianglePos[2] ���_�̈�̃`�F�b�N
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[2], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex2;
				hitFlg = true;
			}
		}

		// trianglePos[0]-trianglePos[1] �ӗ̈�̃`�F�b�N
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], trianglePos[1], sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge01;
				hitFlg = true;
			}
		}

		// trianglePos[0]-trianglePos[2] �ӗ̈�̃`�F�b�N
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], trianglePos[2], sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge02;
				hitFlg = true;
			}
		}

		// trianglePos[1]-trianglePos[2] �ӗ̈�̃`�F�b�N
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[1], trianglePos[2], sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge12;
				hitFlg = true;
			}
		}

		// �������m�肵�Aresult���L���Ȃ�HitResult�����Z�o����
		if (hitFlg && result)
		{
			result->distance = minDistance;
			DirectX::XMStoreFloat3(&result->triangleVerts[0], trianglePos[0]);
			DirectX::XMStoreFloat3(&result->triangleVerts[1], trianglePos[1]);
			DirectX::XMStoreFloat3(&result->triangleVerts[2], trianglePos[2]);
			DirectX::XMStoreFloat3(&result->normal, DirectX::XMVector3Normalize(minNormal));

			// ��_(result->position)�̎Z�o�͓�������ɂ���ĕ���
			switch (minDistCalcPattern)
			{
			case IntersectPattern::enVertex0:
				DirectX::XMStoreFloat3(&result->position, trianglePos[0]);
				break;
			case IntersectPattern::enVertex1:
				DirectX::XMStoreFloat3(&result->position, trianglePos[1]);
				break;
			case IntersectPattern::enVertex2:
				DirectX::XMStoreFloat3(&result->position, trianglePos[2]);
				break;
			case IntersectPattern::enEdge01:
			case IntersectPattern::enEdge02:
			case IntersectPattern::enEdge12:
				DirectX::XMStoreFloat3(&result->position, minOnCenterLinePos);
				break;

			default:
				break;
			}
		}
	}

	return hitFlg;
}

// AABB�ƕ��ʂ̍ŒZ�������擾����	���Ԃ�l���}�C�i�X���ƌ������Ă���󋵂ƒ�`
float Collision::GetMinDist_AABBPlane(
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const DirectX::XMVECTOR& planeNorm,	// �v���K��
	const float planeDist,
	bool* reverse)		// �@���ɑ΂��ė����\���𔻒f����悤�̃t���O
{
	// planeNorm�̊e���̐�Βl������ē���
	float projRadii;
	float dist;
	float ret = 0.0f;




	if (reverse)
	{

	}

	return ret;
}

// AABBvs�O�p�`�̌�������
bool Collision::IntersectAABBVsTriangle(
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const DirectX::XMVECTOR trianglePos[3],
	IntersectionResult* result)
{
	// �v�Z�̍������̂��߁AAABB�����_�Ɉړ����Ĕ�����s��
	// ���̂��ߎO�p�`�����킹�Ĉړ�������B���킹�Đ����v�Z�����C���ɂȂ邽��FLOAT3�ɂ��Ă����B
	DirectX::XMFLOAT3 movedTrianglePos[3] = {};
	DirectX::XMFLOAT3 triangleEdge[3] = {};


	// �O�ϕ������̔���
	// // �v�Z�p�ϐ�
	float proj1 = 0.0f;				// �O�p�`�̎ˉe���P�@�i�O�ϕ������ƒ�������ӂ��\�����钸�_�j
	float proj2 = 0.0f;				// �O�p�`�̎ˉe���Q�@�i�O�ϕ������ƒ�������ӂ��\�����Ȃ����_�j
	float r = 0.0f;					// AABB�̎ˉe��
	float penetration = 0.0f;		// �v�Z���̂߂荞�ݗ�
	float minPenetration = FLT_MAX;	// �ŏ��߂荞�ݗ�
	int minType = -1;				// �ŏ�������Ԃm��
	DirectX::XMVECTOR norm = {}, minNorm = {};
	float axisLength = 0.0f;		// �O�ϕ�������

	// xNorm �~ Edge_N



	// yNorm �~ Edge_N



	// zNorm �~ Edge_N



	// AABB�̖ʖ@�����̔���
	// xNorm+


	// xNorm-


	// yNorm+


	// yNorm-


	// zNorm+


	// zNorm-



	// �O�p�`�̖ʖ@�����̔���
	DirectX::XMVECTOR planeNorm;
	bool reverseChk = true;
	float planeDist = 0.0f;
	penetration = -GetMinDist_AABBPlane({}, aabbRadii, planeNorm, planeDist, &reverseChk);
	if (penetration < 0.0f) return false;	// ���̌v�Z��penetration�͐G��Ă���i==0.0f�j�ł�OK�����A�Ō�̂����͐G��Ă��邾���Ȃ�߂荞��ł��Ȃ�����ɂ���
	if (result && penetration > 0.0f && penetration < minPenetration)
	{
		minPenetration = penetration;
		minType = 15;
	}

	// �@���Z�o
	if (result)
	{
		switch (minType)
		{
		case -1: return false;
		case 9:
			result->normal = { 1.0f,0.0f,0.0f }; break;
		case 10:
			result->normal = { -1.0f,0.0f,0.0f }; break;
		case 11:
			result->normal = { 0.0f,1.0f,0.0f }; break;
		case 12:
			result->normal = { 0.0f,-1.0f,0.0f }; break;
		case 13:
			result->normal = { 0.0f,0.0f,1.0f }; break;
		case 14:
			result->normal = { 0.0f,0.0f,-1.0f }; break;
		case 15:
			result->normal = reverseChk ? DirectX::XMVectorNegate(planeNorm) : planeNorm;
			break;
		default:
			result->normal = DirectX::XMVector3Normalize(minNorm);

			// AABB�ƎO�p�`�̒��S�ԃx�N�g������@���̕�����␳
			if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(result->normal, DirectX::XMLoadFloat3(&movedTrianglePos[0]))) > 0.0f)
			{
				result->normal = DirectX::XMVectorNegate(result->normal);
			}

			break;
		}

		result->penetration = minPenetration;
	}

	return true;
}


// AABBvsAABB�̌�������
bool Collision::IntersectAABBVsAABB(
	const DirectX::XMVECTOR& aabbPos1,
	const DirectX::XMVECTOR& aabbRadii1,
	const DirectX::XMVECTOR& aabbPos2,
	const DirectX::XMVECTOR& aabbRadii2,
	IntersectionResult* result)
{
	float dx = DirectX::XMVectorGetX(aabbRadii1) + DirectX::XMVectorGetX(aabbRadii2) - fabsf(DirectX::XMVectorGetX(aabbPos1) - DirectX::XMVectorGetX(aabbPos2));
	if (dx < 0.0f)
	{
		return false;
	}

	float dy = DirectX::XMVectorGetY(aabbRadii1) + DirectX::XMVectorGetY(aabbRadii2) - fabsf(DirectX::XMVectorGetY(aabbPos1) - DirectX::XMVectorGetY(aabbPos2));
	if (dy < 0.0f)
	{
		return false;
	}

	float dz = DirectX::XMVectorGetZ(aabbRadii1) + DirectX::XMVectorGetZ(aabbRadii2) - fabsf(DirectX::XMVectorGetZ(aabbPos1) - DirectX::XMVectorGetZ(aabbPos2));
	if (dz < 0.0f)
	{
		return false;
	}

	if (result)
	{
		float minAxis = std::min(std::min(dx, dy), dz);
		if (minAxis == dx)
		{
			result->normal = (DirectX::XMVectorGetX(aabbPos1) > DirectX::XMVectorGetX(aabbPos2)) ? DirectX::XMVECTOR{ 1.0f, 0.0f, 0.0f } : DirectX::XMVECTOR{ -1.0f, 0.0f, 0.0f };
			result->penetration = dx;
		}
		else if (minAxis == dy)
		{
			result->normal = (DirectX::XMVectorGetY(aabbPos1) > DirectX::XMVectorGetY(aabbPos2)) ? DirectX::XMVECTOR{ 0.0f, 1.0f, 0.0f } : DirectX::XMVECTOR{ 0.0f, -1.0f, 0.0f };
			result->penetration = dy;
		}
		else if (minAxis == dz)
		{
			result->normal = (DirectX::XMVectorGetZ(aabbPos1) > DirectX::XMVectorGetZ(aabbPos2)) ? DirectX::XMVECTOR{ 0.0f, 0.0f, 1.0f } : DirectX::XMVECTOR{ 0.0f, 0.0f, -1.0f };
			result->penetration = dz;
		}

		result->pointA = DirectX::XMVectorSubtract(aabbPos1, DirectX::XMVectorMultiply(aabbRadii1, result->normal));
		result->pointB = DirectX::XMVectorAdd(aabbPos2, DirectX::XMVectorMultiply(aabbRadii2, result->normal));
	}

	return true;
}


// �����Ɛ����̍ŒZ�����̓����擾����
float Collision::GetMinDistSq_SegmentSegment(
	const  DirectX::XMVECTOR& point1A,
	const  DirectX::XMVECTOR& point1B,
	const  DirectX::XMVECTOR& point2A,
	const  DirectX::XMVECTOR& point2B,
	DirectX::XMVECTOR* nearPoint1,
	DirectX::XMVECTOR* nearPoint2)
{
	DirectX::XMVECTOR segmentDirection1 = DirectX::XMVectorSubtract(point1B, point1A);
	DirectX::XMVECTOR segmentDirection2 = DirectX::XMVectorSubtract(point2B, point2A);
	DirectX::XMVECTOR r = DirectX::XMVectorSubtract(point1A, point2A);

	float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection1));
	float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, segmentDirection2));
	float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, r));

	float t1 = 0.0f, t2 = 0.0f;

	if (a <= FLT_EPSILON && e <= FLT_EPSILON)	// ���������_�ɏk�ނ��Ă���ꍇ
	{
		t1 = t2 = 0.0f;
	}
	else if (a <= FLT_EPSILON)					// �Е��id0�j���_�ɏk�ނ��Ă���ꍇ
	{
		t1 = 0.0f;
		t2 = std::clamp(f / e, 0.0f, 1.0f);
	}
	else
	{
		float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, r));
		if (e <= FLT_EPSILON)					// �Е��id1�j���_�ɏk�ނ��Ă���ꍇ
		{
			t2 = 0.0f;
			t1 = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else									// ����������
		{
			float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection2));
			float denom = a * e - b * b;

			if (denom != 0.0f)					// ���s�m�F�i���s���� t1 = 0.0f�i�����̎n�[�j�����̏����l�Ƃ��Čv�Z�������߂�j
			{
				t1 = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}

			t2 = b * t1 + f;

			if (t2 < 0.0f)						// t1���n�[���O���ɂ���ꍇ
			{
				t2 = 0.0f;
				t1 = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t2 > e)					// t1���I�[���O���ɂ���ꍇ
			{
				t2 = 1.0f;
				t1 = std::clamp((b - c) / a, 0.0f, 1.0f);
			}
			else								// t1��������ɂ���ꍇ
			{
				t2 /= e;
			}
		}
	}

	// �e������̍ŋߓ_�Z�o
	DirectX::XMVECTOR point1 = DirectX::XMVectorAdd(point1A, DirectX::XMVectorScale(segmentDirection1, t1));
	DirectX::XMVECTOR point2 = DirectX::XMVectorAdd(point2A, DirectX::XMVectorScale(segmentDirection2, t2));

	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);

	if (nearPoint1)
	{
		*nearPoint1 = point1;
	}
	if (nearPoint2)
	{
		*nearPoint2 = point2;
	}

	return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));
}

// �����ƎO�p�`�̍ŒZ�����̓����擾����
float Collision::GetMinDistSq_SegmentTriangle(
	const  DirectX::XMVECTOR& pointA,			// �����[A
	const  DirectX::XMVECTOR& pointB,			// �����[B
	const  DirectX::XMVECTOR trianglePos[3],	// �O�p�`���_
	DirectX::XMVECTOR* nearPointSegment,		// �������̍ŋߓ_
	DirectX::XMVECTOR* nearPointTriangle,		// �O�p�`���̍ŋߓ_
	bool& nearPointEdghFlg)						// �Ӂi���_�j���ŋߓ_�̏ꍇtrue
{
	// ��������ʂ��ŋߓ_�Ƃ��Ă���
	nearPointEdghFlg = false;

	// �������O�p�`�ƌ������邩�m�F
	DirectX::XMVECTOR rayDirection;
	float rayDist;





	// �����̊e���_�ɑ΂��A�O�p�`�̍ŋߓ_�����߁A���킹�ĎO�p�`�̓����ɂ��邩�O���ɂ��邩�m�F
	DirectX::XMVECTOR nearPosA = {};
	bool insideA = GetClosestPoint_PointTriangle(pointA, trianglePos, nearPosA);
	DirectX::XMVECTOR nearPosB = {};
	bool insideB = GetClosestPoint_PointTriangle(pointB, trianglePos, nearPosB);


	// �����ꂩ�̓_���ˉe�����_���O�p�`�����Ɏ��܂�ꍇ�A���̓_���ŒZ�_�ɂȂ�\�����l���A�������L�^����
	// �܂��A�����̓_���O�p�`���܂ޕ��ʂ����E�ɓ������ɑ��݂��A�ˉe�����ꍇ�ɎO�p�`�����Ɏ��܂�ꍇ��
	// �������Ă��Ȃ����߁A�����̂ǂ��炩�̒[�_���ŒZ�ӏ��ɂȂ菈���I��
	float nearDistSq = FLT_MAX;
	if (insideA || insideB)
	{





	}

	// �����܂ŗ����i�K�ŁA�Ӂi���_���܂ށj�ł̌������m�肷�邽�߁AnearPointEdghFlg��true�ɂ��Ă���
	// (�J�v�Z���ƎO�p�`�̌��������̖@���̎Z�o�ɖʂƂ̌����Ȃ̂��A�Ӂi���_�j�Ƃ̌����Ȃ̂��̏�񂪕K�v�Ȃ���)
	nearPointEdghFlg = true;

	// �O�p�`�O���ɐ����i�܂��͒[�j�����݂��A�ŋߓ_���ӏ�ɂȂ�ꍇ�A�����ƎO�p�`�̂R�ӂ̋�����S�Čv������r����





	return nearDistSq;
}

// �J�v�Z��vs�O�p�`�̌�������
bool Collision::IntersectCapsuleVsTriangle(
	const DirectX::XMVECTOR& position,
	const DirectX::XMVECTOR& direction,
	float radius,
	float length,
	const DirectX::XMVECTOR trianglePos[3],
	IntersectionResult* result)
{
	DirectX::XMVECTOR nearPointSegment = {};
	DirectX::XMVECTOR nearPointTriangle = {};

	bool ret = false;
	DirectX::XMVECTOR end = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, length));
	bool nearPointEdghFlg;

	// �����ƎO�p�`�̋������v�Z����
	float distSq = GetMinDistSq_SegmentTriangle(position, end, trianglePos, &nearPointSegment, &nearPointTriangle, nearPointEdghFlg);
	if (distSq < radius * radius)
	{
		ret = true;
	}

	// �Փˏ����≟���߂������p��result�Ƀp�����[�^��ݒ肷��
	if (result)
	{
		// pointA,pointB�ݒ�
		result->pointA = nearPointSegment;
		result->pointB = nearPointTriangle;

		if (ret)
		{
			// normal, penetration�ݒ�
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(result->pointA, result->pointB);
			if (!nearPointEdghFlg)
			{
				// �ŒZ�_���ʏ�̏ꍇ



			}
			else
			{
				// �ŒZ�_���ӏ�̏ꍇ



			}
		}
	}

	return ret;
}

// �J�v�Z��vs�J�v�Z���̌�������
bool Collision::IntersectCapsuleVsCapsule(
	const DirectX::XMVECTOR& position1,	// ���S
	const DirectX::XMVECTOR& direction1,	// �����i���K���j
	const float					length1,	// ����
	const float					radius1,	// ���a
	const DirectX::XMVECTOR& position2,	// ���S
	const DirectX::XMVECTOR& direction2,	// �����i���K���j
	const float					length2,	// ����
	const float					radius2,	// ���a
	IntersectionResult* result)
{
	// �e�J�v�Z���̒��S����̍ŋߓ_�Z�o
	DirectX::XMVECTOR point1 = {}, point2 = {};
	DirectX::XMVECTOR end1 = DirectX::XMVectorAdd(position1, DirectX::XMVectorScale(direction1, length1));
	DirectX::XMVECTOR end2 = DirectX::XMVectorAdd(position2, DirectX::XMVectorScale(direction2, length2));

	float distSq = GetMinDistSq_SegmentSegment(position1, end1, position2, end2, &point1, &point2);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);
	float radiusAdd = radius1 + radius2;

	if (distSq < radiusAdd * radiusAdd)
	{
		if (result)
		{
			result->normal = DirectX::XMVector3Normalize(vec);
			result->penetration = radiusAdd - sqrtf(distSq);
			result->pointA = DirectX::XMVectorAdd(point1, DirectX::XMVectorScale(result->normal, -radius1));
			result->pointB = DirectX::XMVectorAdd(point2, DirectX::XMVectorScale(result->normal, radius2));
		}

		return true;
	}

	return false;
}


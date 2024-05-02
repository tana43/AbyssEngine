#include "FbxMeshData.h"
#include "Misc.h"
#include "Shader.h"
#include "Texture.h"

#include <sstream>
#include <fstream>
#include <functional>
#include <filesystem>

#include "imgui/imgui.h"

using namespace AbyssEngine;

struct BoneInfluence
{
    uint32_t boneIndex;
    float boneWeight;
};
using BoneInfluencePerControlPoint = std::vector<BoneInfluence>;

//�{�[���e���x���擾����֐�
void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<BoneInfluencePerControlPoint>& boneInfluences)
{
    const int controlPointsCount{ fbxMesh->GetControlPointsCount() };
    boneInfluences.resize(controlPointsCount);

    const int skinCount{ fbxMesh->GetDeformerCount(FbxDeformer::eSkin) };
    for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
    {
        const FbxSkin* fbxSkin{ static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex,FbxDeformer::eSkin)) };

        const int clusterCount{ fbxSkin->GetClusterCount() };
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {
            const FbxCluster* fbxCluster{ fbxSkin->GetCluster(clusterIndex) };

            const int controlPointIndicesCount{ fbxCluster->GetControlPointIndicesCount() };
            for (int controlPointIndicesIndex = 0;
                controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
            {
                int controlPointIndex{ fbxCluster->GetControlPointIndices()[controlPointIndicesIndex] };
                double controlPointWeight{ fbxCluster->GetControlPointWeights()[controlPointIndicesIndex] };
                BoneInfluence& boneInfluenc{ boneInfluences.at(controlPointIndex).emplace_back() };
                boneInfluenc.boneIndex = static_cast<uint32_t>(clusterIndex);
                boneInfluenc.boneWeight = static_cast<float>(controlPointWeight);
            }
        }
    }
}

FbxMeshData::FbxMeshData(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate)
{
    //�V���A���C�Y���ꂽ�f�[�^������ꍇ�����炩�烍�[�h�A�Ȃ���Ώ]���ʂ�FBX���烍�[�h
    std::filesystem::path cerealFilename(fbxFilename);
    cerealFilename.replace_extension("cereal");
    if (std::filesystem::exists(cerealFilename.c_str()))
    {
        std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(sceneView_, meshes_, materials_, animationClips_);
    }
    else
    {
        FbxManager* fbxManager{ FbxManager::Create() };
        FbxScene* fbxScene{ FbxScene::Create(fbxManager,"") };

        FbxImporter* fbxImporter{ FbxImporter::Create(fbxManager,"") };
        bool importStatus{ false };
        importStatus = fbxImporter->Initialize(fbxFilename);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        importStatus = fbxImporter->Import(fbxScene);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        FbxGeometryConverter fbxConverter(fbxManager);
        if (triangulate)
        {
            fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
            fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
        }

        std::function<void(FbxNode*)> traverse{[&](FbxNode* fbxNode)
            {
                ModelScene::Node& Node{sceneView_.nodes_.emplace_back()};
                Node.attribute_ = fbxNode->GetNodeAttribute() ?
                    fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                Node.name_ = fbxNode->GetName();
                Node.uniqueId_ = fbxNode->GetUniqueID();
                Node.parentIndex_ = sceneView_.indexOf(fbxNode->GetParent() ?
                    fbxNode->GetParent()->GetUniqueID() : 0);
                for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
                {
                    traverse(fbxNode->GetChild(childIndex));
                }
        } };
        traverse(fbxScene->GetRootNode());

 #if 0
        for (const Scene::Node& node : sceneView.nodes)
        {
            FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
            //Display node data in the output window as debug
            std::string nodeName = fbxNode->GetName();
            uint64_t uid = fbxNode->GetUniqueID();
            uint64_t parentUid = fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0;
            int32_t type = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : 0;

            std::stringstream debugString;
            debugString << nodeName << ":" << uid << ":" << parentUid << ":" << type << "\n";
            OutputDebugStringA(debugString.str().c_str());
        }
#endif // 1

        FetchMeshes(fbxScene, meshes_);

        FetchMaterials(fbxScene, materials_);

#if 0
        float samplingRate = 0;
#endif // 0

        FetchAnimations(fbxScene, animationClips_, samplingRate);

        fbxManager->Destroy();

        std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(sceneView_, meshes_, materials_, animationClips_);
    }



    CreateComObjects(device, fbxFilename);
}

inline DirectX::XMFLOAT4X4 ToXMFloat4x4(const FbxAMatrix& fbxamatrix)
{
    DirectX::XMFLOAT4X4 xmfloat4x4;
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
        }
    }
    return xmfloat4x4;
}
inline DirectX::XMFLOAT3 ToXMFloat3(const FbxDouble3& fbxdouble3)
{
    DirectX::XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}
inline DirectX::XMFLOAT4 ToXMFloat4(const FbxDouble4& fbxdouble4)
{
    DirectX::XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}

void FbxMeshData::UpdateAnimation(Animation::Keyframe& keyframe)
{
    size_t nodeCount{ keyframe.nodes_.size() };
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        Animation::Keyframe::Node& Node{keyframe.nodes_.at(nodeIndex)};
        DirectX::XMMATRIX S{DirectX::XMMatrixScaling(Node.scaling_.x, Node.scaling_.y, Node.scaling_.z)};
        DirectX::XMMATRIX R{DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&Node.rotation_))};
        DirectX::XMMATRIX T{DirectX::XMMatrixTranslation(Node.translation_.x, Node.translation_.y, Node.translation_.z)};

        int64_t parentIndex{ sceneView_.nodes_.at(nodeIndex).parentIndex_ };
        DirectX::XMMATRIX P{parentIndex < 0 ? DirectX::XMMatrixIdentity() :
            DirectX::XMLoadFloat4x4(&keyframe.nodes_.at(parentIndex).globalTransform_)};

        DirectX::XMStoreFloat4x4(&Node.globalTransform_, S * R * T * P);
    }
}

void FbxMeshData::FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes)
{
    for (const ModelScene::Node& Node : sceneView_.nodes_)
    {
        if (Node.attribute_ != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        FbxNode* fbxNode{ fbxScene->FindNodeByName(Node.name_.c_str()) };
        FbxMesh* fbxMesh{ fbxNode->GetMesh() };

        Mesh& mesh_{ meshes.emplace_back() };

#if 0
        mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
        mesh.name = fbxMesh->GetNode()->GetName();
        mesh.nodeIndex = sceneView.indexOf(mesh.uniqueId);
        mesh.defaultGlobalTransform = ToXMFloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());
#else
        mesh_.uniqueId_ = Node.uniqueId_;
        mesh_.name_ = Node.name_;
        mesh_.nodeIndex_ = sceneView_.indexOf(Node.uniqueId_);
        mesh_.defaultGlobalTransform_ = ToXMFloat4x4(fbxNode->EvaluateGlobalTransform());
#endif // 0



        //�{�[���e���x�擾
        std::vector<BoneInfluencePerControlPoint> boneInfluences;
        FetchBoneInfluences(fbxMesh, boneInfluences);

        //�o�C���h�|�[�Y�擾(�����|�[�Y)
        FetchSkeleton(fbxMesh, mesh_.bindPose_);

        std::vector<Mesh::Subset>& subsets{mesh_.subsets_};
        const int materialCount{ fbxMesh->GetNode()->GetMaterialCount() };
        subsets.resize(materialCount > 0 ? materialCount : 1);
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
        {
            const FbxSurfaceMaterial* fbxMaterial{ fbxMesh->GetNode()->GetMaterial(materialIndex) };
            subsets.at(materialIndex).materialName_ = fbxMaterial->GetName();
            subsets.at(materialIndex).materialUniqueId_ = fbxMaterial->GetUniqueID();
        }
        if (materialCount > 0)
        {
            const int polygonCount{ fbxMesh->GetPolygonCount() };
            for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
            {
                const int materialIndex{
                    fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) };
                subsets.at(materialIndex).indexCount_ += 3;
            }
            uint32_t offset{ 0 };
            for (Mesh::Subset& subset : subsets)
            {
                subset.startIndexLocation_ = offset;
                offset += subset.indexCount_;
                subset.indexCount_ = 0;
            }
        }

        const int polygonCount{ fbxMesh->GetPolygonCount() };
        mesh_.vertices_.resize(polygonCount * 3LL);
        mesh_.indices_.resize(polygonCount * 3LL);

        FbxStringList uvNames;
        fbxMesh->GetUVSetNames(uvNames);
        const FbxVector4* controlPoints{ fbxMesh->GetControlPoints() };
        for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
        {
            if (polygonIndex % 1000 == 0)
            {
                int i = 1;
            }

            const int materialIndex{ materialCount > 0 ?
            fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0 };
            Mesh::Subset& subset{subsets.at(materialIndex)};
            const uint32_t offset{ subset.startIndexLocation_ + subset.indexCount_ };

            for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
            {
                const int vertexIndex{ polygonIndex * 3 + positionInPolygon };

                Vertex vertex;
                const int polygonVertex{ fbxMesh->GetPolygonVertex(polygonIndex,positionInPolygon) };
                vertex.position_.x = static_cast<float>(controlPoints[polygonVertex][0]);
                vertex.position_.y = static_cast<float>(controlPoints[polygonVertex][1]);
                vertex.position_.z = static_cast<float>(controlPoints[polygonVertex][2]);


                //�E�F�C�g�l�𒸓_�ɃZ�b�g
                const BoneInfluencePerControlPoint& influencePerControlPoint{ boneInfluences.at(polygonVertex) };
                for (size_t influenceIndex = 0; influenceIndex < influencePerControlPoint.size(); ++influenceIndex)
                {

                    //const size_t maxBoneInfluence{ influencePerControlPoint.max_size() };
                    if (influenceIndex < MAX_BONE_INFLUENCES)
                    {
                        vertex.boneWeights_[influenceIndex] =
                            influencePerControlPoint.at(influenceIndex).boneWeight;
                        vertex.boneIndices_[influenceIndex] =
                            influencePerControlPoint.at(influenceIndex).boneIndex;
                    }
                    else
                    {
                        _ASSERT_EXPR(FALSE, L"�e������{�[������4�ȏ㑶�݂��Ă���");

                        //���̏ꍇ�̏������l���Ă݂�
                    }
                    //�S��weight�̃g�[�^���͕K��1.0�ɂȂ��Ă��Ȃ��Ƃ����Ȃ��I
                }

                if (fbxMesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
                    vertex.normal_.x = static_cast<float>(normal[0]);
                    vertex.normal_.y = static_cast<float>(normal[1]);
                    vertex.normal_.z = static_cast<float>(normal[2]);
                }
                if (fbxMesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmappedUv;
                    fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon,
                        uvNames[0], uv, unmappedUv);
                    vertex.texcoord_.x = static_cast<float>(uv[0]);
                    vertex.texcoord_.y = 1.0f - static_cast<float>(uv[1]);
                }

                //�@���x�N�g���̒l���擾
                if (fbxMesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
                    _ASSERT_EXPR(tangent->GetMappingMode() == FbxGeometryElement::EMappingMode::eByPolygonVertex &&
                        tangent->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eDirect,
                        L"Only supports a combination of these modes.");

                    vertex.tangent_.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
                    vertex.tangent_.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
                    vertex.tangent_.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
                    vertex.tangent_.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
                }

                mesh_.vertices_.at(vertexIndex) = std::move(vertex);
                mesh_.indices_.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
                subset.indexCount_++;
            }
        }

        //�o�E���f�B���O�{�b�N�X�쐬
        for (const Vertex& v : mesh_.vertices_)
        {
            mesh_.boundingBox_[0].x = std::min<float>(mesh_.boundingBox_[0].x, v.position_.x);
            mesh_.boundingBox_[0].y = std::min<float>(mesh_.boundingBox_[0].y, v.position_.y);
            mesh_.boundingBox_[0].z = std::min<float>(mesh_.boundingBox_[0].z, v.position_.z);
            mesh_.boundingBox_[1].x = std::max<float>(mesh_.boundingBox_[1].x, v.position_.x);
            mesh_.boundingBox_[1].y = std::max<float>(mesh_.boundingBox_[1].y, v.position_.y);
            mesh_.boundingBox_[1].z = std::max<float>(mesh_.boundingBox_[1].z, v.position_.z);
        }
    }
}

void FbxMeshData::FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials)
{
    const size_t nodeCount{ sceneView_.nodes_.size() };
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        const ModelScene::Node& Node{sceneView_.nodes_.at(nodeIndex)};
        const FbxNode* fbxNode{ fbxScene->FindNodeByName(Node.name_.c_str()) };

        const int materialCount{ fbxNode->GetMaterialCount() };
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
        {
            const FbxSurfaceMaterial* fbxMaterial{ fbxNode->GetMaterial(materialIndex) };

            Material material_;
            material_.name_ = fbxMaterial->GetName();
            material_.uniqueId_ = fbxMaterial->GetUniqueID();
            FbxProperty fbxProperty;
            //�f�B�t���[�Y�擾(�g�U���ˌ��A�U������\���j
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color_{ fbxProperty.Get<FbxDouble3>() };
                material_.Kd_.x = static_cast<float>(color_[0]);
                material_.Kd_.y = static_cast<float>(color_[1]);
                material_.Kd_.z = static_cast<float>(color_[2]);
                material_.Kd_.w = 1.0f;

                const FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                material_.textureFilenames_[0] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }
            //�A���r�G���g�擾(�����A�e�I�u�W�F�N�g�ɕʁX�̃p�����[�^��ݒ肷��ƃL���Ȃ�炵��)
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color_{ fbxProperty.Get<FbxDouble3>() };
                material_.Ka_.x = static_cast<float>(color_[0]);
                material_.Ka_.y = static_cast<float>(color_[1]);
                material_.Ka_.z = static_cast<float>(color_[2]);
                material_.Ka_.w = 1.0f;

                //const FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                //material.textureFilenames[0] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }
            //�X�y�L�����擾(���ʔ��ˌ��A�c����\������@�l�H���ȊO�ɂ͂��܂�g��Ȃ���������)
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color_{ fbxProperty.Get<FbxDouble3>() };
                material_.Ks_.x = static_cast<float>(color_[0]);
                material_.Ks_.y = static_cast<float>(color_[1]);
                material_.Ks_.z = static_cast<float>(color_[2]);
                material_.Ks_.w = 1.0f;

                //const FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                //material.textureFilenames[0] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }

            //�@���}�b�v�̃t�@�C�������擾
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbxProperty.IsValid())
            {
                const FbxFileTexture* fileTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                material_.textureFilenames_[1] = fileTexture ? fileTexture->GetRelativeFileName() : "";
            }

            //�G�~�b�V�����e�N�X�`���擾
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbxProperty.IsValid())
            {
                const FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                material_.textureFilenames_[2] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }
            materials.emplace(material_.uniqueId_, std::move(material_));
        }
        if (materialCount == 0)
        {
            Material dummy{};
            materials.emplace(0, dummy);
        }
    }
}

//���b�V������o�C���h�|�[�Y�̏��𒊏o
void FbxMeshData::FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose)
{
    //���b�V������X�L�����擾
    const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
    {
        FbxSkin* skin_ = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

        //�X�L������N���X�^�[�i���_�e���͂Ƃ���ɑΉ�����{�[���j���擾
        const int clusterCount = skin_->GetClusterCount();
        bindPose.bones_.resize(clusterCount);
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {
            FbxCluster* cluster = skin_->GetCluster(clusterIndex);

            Skeleton::Bone& bone{bindPose.bones_.at(clusterIndex)};
            bone.name_ = cluster->GetLink()->GetName();
            bone.uniqueId_ = cluster->GetLink()->GetUniqueID();
            bone.parentIndex_ = bindPose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.nodeIndex_ = sceneView_.indexOf(bone.uniqueId_);

            //���b�V���̃��[�J����Ԃ���V�[���̃O���[�o����Ԃɕϊ����邽�߂Ɏg��
            FbxAMatrix referenceGlobalInitPosition;
            cluster->GetTransformMatrix(referenceGlobalInitPosition);

            //�{�[���̃��[�J����Ԃ���V�[���̃O���[�o����Ԃɕϊ����邽�߂Ɏg��
            FbxAMatrix clusterGlobalInitPosition;
            cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

            //�I�t�Z�b�g�s��̍쐬
            //�����b�V����Ԃ���{�[����Ԃւ̌���������s����I�t�Z�b�g�s��ƌĂ�
            bone.offsetTransform_
                = ToXMFloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
        }
    }
}

//�A�j���[�V������񒊏o
void FbxMeshData::FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips,
    float samplingRate/*�P�b�ԂɃA�j���[�V���������o���񐔁B���̒l���O�̏ꍇ�̓A�j���[�V�����f�[�^�̓f�t�H���g�̃t���[�����[�g�ŃT���v�����O�����*/)
{
    FbxArray<FbxString*> animationStackNames;
    fbxScene->FillAnimStackNameArray(animationStackNames);
    const int animationStackCount{ animationStackNames.GetCount() };
    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
    {
        Animation& animationClip{ animationClips.emplace_back() };
        animationClip.name_ = animationStackNames[animationStackIndex]->Buffer();

        FbxAnimStack* animationStack{ fbxScene->FindMember<FbxAnimStack>(animationClip.name_.c_str()) };
        fbxScene->SetCurrentAnimationStack(animationStack);

        //�P�b��FbxTime�ɕϊ�
        const FbxTime::EMode timeMode{fbxScene->GetGlobalSettings().GetTimeMode()};
        FbxTime oneSecond;
        oneSecond.SetTime(0/*����*/, 0/*��*/, 1/*�b*/, 0, 0, timeMode);

        //�T���v�����O���[�g���O�Ȃ�f�t�H���g�l���擾
        animationClip.samplingRate_ = samplingRate > 0 ?
            samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
        const FbxTime samplingInterval{ static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate_) };

        //�A�j���[�V���������擾
        const FbxTakeInfo* takeInfo{ fbxScene->GetTakeInfo(animationClip.name_.c_str()) };

        const FbxTime startTime{ takeInfo->mLocalTimeSpan.GetStart() };
        const FbxTime stopTime{ takeInfo->mLocalTimeSpan.GetStop() };
        for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
        {
            Animation::Keyframe& keyframe{animationClip.sequence_.emplace_back()};

            const size_t nodeCount{ sceneView_.nodes_.size() };
            keyframe.nodes_.resize(nodeCount);
            for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                FbxNode* fbxNode{ fbxScene->FindNodeByName(sceneView_.nodes_.at(nodeIndex).name_.c_str()) };
                if (fbxNode)
                {
                    Animation::Keyframe::Node& Node{keyframe.nodes_.at(nodeIndex)};

                    //�O���[�o�����W�n�ւ̌����s��
                    Node.globalTransform_ = ToXMFloat4x4(fbxNode->EvaluateGlobalTransform(time));

                    //parent�̃��[�J�����W�n�Ɋւ���ϊ��s��
                    const FbxAMatrix& localTransform{ fbxNode->EvaluateLocalTransform(time) };
                    Node.scaling_ = ToXMFloat3(localTransform.GetS());
                    Node.rotation_ = ToXMFloat4(localTransform.GetQ());
                    Node.translation_ = ToXMFloat3(localTransform.GetT());
                }
            }
        }
    }
    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
    {
        delete animationStackNames[animationStackIndex];
    }
}

void FbxMeshData::BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe)
{
    size_t nodeCount{ keyframes[0]->nodes_.size() };
    keyframe.nodes_.resize(nodeCount);
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        DirectX::XMVECTOR S[2]{
            DirectX::XMLoadFloat3(&keyframes[0]->nodes_.at(nodeIndex).scaling_),
            DirectX::XMLoadFloat3(&keyframes[1]->nodes_.at(nodeIndex).scaling_)
        };
        DirectX::XMStoreFloat3(&keyframe.nodes_.at(nodeIndex).scaling_, DirectX::XMVectorLerp(S[0], S[1], factor));

        DirectX::XMVECTOR R[2]{
            DirectX::XMLoadFloat4(&keyframes[0]->nodes_.at(nodeIndex).rotation_),
            DirectX::XMLoadFloat4(&keyframes[1]->nodes_.at(nodeIndex).rotation_)
        };
        DirectX::XMStoreFloat4(&keyframe.nodes_.at(nodeIndex).rotation_, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

        DirectX::XMVECTOR T[2]{
            DirectX::XMLoadFloat3(&keyframes[0]->nodes_.at(nodeIndex).translation_),
            DirectX::XMLoadFloat3(&keyframes[1]->nodes_.at(nodeIndex).translation_)
        };
        DirectX::XMStoreFloat3(&keyframe.nodes_.at(nodeIndex).translation_, DirectX::XMVectorLerp(T[0], T[1], factor));
    }
}

bool FbxMeshData::AppendAnimations(const char* animationFilename, float samplingRate)
{
    //�V���A���C�Y���ꂽ�f�[�^������ꍇ�����炩�烍�[�h�A�˂���Ώ]���ʂ�FBX���烍�[�h
    std::filesystem::path cerealFilename(animationFilename);
    cerealFilename.replace_extension("cereal");
    if (std::filesystem::exists(cerealFilename.c_str()))
    {
        std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(animationClips_);
    }
    else
    {
        //�ʃt�@�C������A�j���[�V�������o
        FbxManager* fbxManager{ FbxManager::Create() };
        FbxScene* fbxScene{ FbxScene::Create(fbxManager,"") };

        FbxImporter* fbxImporter{ FbxImporter::Create(fbxManager,"") };
        bool importStatus{ false };
        importStatus = fbxImporter->Initialize(animationFilename);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
        importStatus = fbxImporter->Import(fbxScene);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        FetchAnimations(fbxScene, animationClips_, samplingRate);

        fbxManager->Destroy();

        std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(animationClips_);
    }
    return true;
}

void FbxMeshData::CreateComObjects(ID3D11Device* device, const char* fbxFilename)
{
    for (Mesh& mesh_ : meshes_)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC bufferDesc{};
        D3D11_SUBRESOURCE_DATA subresourceData{};
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh_.vertices_.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        subresourceData.pSysMem = mesh_.vertices_.data();
        subresourceData.SysMemPitch = 0;
        subresourceData.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&bufferDesc, &subresourceData,
            mesh_.vertexBuffer_.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh_.indices_.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresourceData.pSysMem = mesh_.indices_.data();
        hr = device->CreateBuffer(&bufferDesc, &subresourceData,
            mesh_.indexBuffer_.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

#if 1
        mesh_.vertices_.clear();
        mesh_.indices_.clear();
#endif // 1
    }

    //�e��e�N�X�`���ǂݍ���
    for (std::unordered_map<uint64_t, Material>::iterator itr = materials_.begin();
        itr != materials_.end(); ++itr)
    {
        for (size_t textureIndex = 0; textureIndex < 3; ++textureIndex)
        {
            if (itr->second.textureFilenames_[textureIndex].size() > 0)
            {
                std::filesystem::path path(fbxFilename);
                path.replace_filename(itr->second.textureFilenames_[textureIndex]);
                itr->second.textures_[textureIndex] = Texture::Load(path.string());
            }
            else
            {
                //�_�~�[�e�N�X�`���̓ǂݍ���
                itr->second.textures_[textureIndex] = Texture::Load("./Scripts/Resources/dummy_texture_00.png");
            }
        }
    }

    HRESULT hr = S_OK;
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
        {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
        {"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
        {"WEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
        {"BONES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,D3D11_APPEND_ALIGNED_ELEMENT},
    };
    vertexShader_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/MeshVS.cso",inputLayout_.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    pixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/MeshPS.cso");

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(Constants);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer_.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void FbxMeshData::Render(ID3D11DeviceContext* immediateContext, const Animation::Keyframe* keyframe)
{
    const DirectX::XMFLOAT4X4 coordinateSystemTransforms[]{
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, // 0:RHS Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, // 1:LHS Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 }, // 2:RHS Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 }, // 3:LHS Z-UP
    };

    DirectX::XMMATRIX C{DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&coordinateSystemTransforms[0]),
        DirectX::XMMatrixScaling(scaleFactor_, scaleFactor_, scaleFactor_))};

    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle_.x,angle_.y,angle_.z);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position_.x,position_.y,position_.z);

    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world,C * S * R * T);
    Render(immediateContext, world, color_, keyframe);
}

void FbxMeshData::Render(
    ID3D11DeviceContext* immediateContext,
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& materialColor,
    const Animation::Keyframe* keyframe)
{
    for (const Mesh& mesh_ : meshes_)
    {
        uint32_t stride{ sizeof(Vertex) };
        uint32_t offset{ 0 };
        immediateContext->IASetVertexBuffers(0, 1, mesh_.vertexBuffer_.GetAddressOf(), &stride, &offset);
        immediateContext->IASetIndexBuffer(mesh_.indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
        immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        immediateContext->IASetInputLayout(inputLayout_.Get());

        immediateContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
        immediateContext->PSSetShader(pixelShader_.Get(), nullptr, 0);

        //immediateContext->PSSetShaderResources(0, 1, materials.cbegin()->second.shaderResourceViews[0].GetAddressOf());

        Constants data;
        if (keyframe && keyframe->nodes_.size() > 0)
        {
            //���b�V����globalTransform�����Ԏ��ŕω����Ă���̂ŁA���̍s����L�[�t���[������擾����
            //�擾����globalTransform�s���萔�o�b�t�@�̃��[���h�����s��ɍ�������
            const Animation::Keyframe::Node& meshNode{keyframe->nodes_.at(mesh_.nodeIndex_)};
            DirectX::XMStoreFloat4x4(&data.world_, DirectX::XMLoadFloat4x4(&meshNode.globalTransform_) * DirectX::XMLoadFloat4x4(&world));

            const size_t boneCount{ mesh_.bindPose_.bones_.size() };
            _ASSERT_EXPR(boneCount < MAX_BONES, L"The value of the 'boneCount' has exceeded MAX_BONES.");

            //�����{�[���̏������[�J������O���[�o���ɕς��Ă鑽������
            for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
            {
                const Skeleton::Bone& bone{mesh_.bindPose_.bones_.at(boneIndex)};
                const Animation::Keyframe::Node& boneNode{keyframe->nodes_.at(bone.nodeIndex_)};
                DirectX::XMStoreFloat4x4(&data.boneTransforms_[boneIndex],
                    DirectX::XMLoadFloat4x4(&bone.offsetTransform_) *
                    DirectX::XMLoadFloat4x4(&boneNode.globalTransform_) *
                    DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mesh_.defaultGlobalTransform_))
                );
            }
        }
        else//�A�j���[�V�����������Ă��Ȃ�
        {
            DirectX::XMStoreFloat4x4(&data.world_,
                DirectX::XMLoadFloat4x4(&mesh_.defaultGlobalTransform_) * DirectX::XMLoadFloat4x4(&world));
            for (size_t boneIndex = 0; boneIndex < MAX_BONES; ++boneIndex)
            {
                data.boneTransforms_[boneIndex] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
            }
        }
        for (const Mesh::Subset& subset : mesh_.subsets_)
        {
            const Material& material_{ materials_.at(subset.materialUniqueId_) };
            DirectX::XMStoreFloat4(&data.materialColor_,
                DirectX::XMVectorMultiply(
                    DirectX::XMLoadFloat4(&materialColor),
                    DirectX::XMLoadFloat4(&material_.Kd_))
            );
            immediateContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &data, 0, 0);
            immediateContext->VSSetConstantBuffers(1, 1, constantBuffer_.GetAddressOf());

#if 0
            immediateContext->PSSetShaderResources(0, 1, material.shaderResourceViews_[0].GetAddressOf());
            immediateContext->PSSetShaderResources(1, 1, material.shaderResourceViews_[1].GetAddressOf());
            //�G�~�b�V�����p�e�N�X�`���Z�b�g
            immediateContext->PSSetShaderResources(2, 1, material.shaderResourceViews_[2].GetAddressOf());
#else
            //�e�N�X�`���o�C���h
            material_.textures_[0]->Set(0, Shader_Type::Pixel);
            material_.textures_[1]->Set(1, Shader_Type::Pixel);
#endif // 0

            immediateContext->DrawIndexed(subset.indexCount_, subset.startIndexLocation_, 0);
        }

        //DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform) * DirectX::XMLoadFloat4x4(&world));
    }
}


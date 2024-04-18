#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include "Asset.h"


namespace AbyssEngine
{
    class Texture;

    struct ModelScene
    {
        struct Node
        {
            uint64_t uniqueId_{ 0 };
            std::string name_;
            FbxNodeAttribute::EType attribute_{FbxNodeAttribute::EType::eUnknown};
            int64_t parentIndex_{ -1 };

            template<class T>
            void serialize(T& archive)
            {
                archive(uniqueId_, name_, attribute_, parentIndex_);
            }
        };
        std::vector<Node> nodes_;
        int64_t indexOf(uint64_t uniqueId)const
        {
            int64_t index{ 0 };
            for (const Node& node : nodes_)
            {
                if (node.uniqueId_ == uniqueId)
                {
                    return index;
                }
                ++index;
            }
            return -1;
        }

        template<class T>
        void serialize(T& archive)
        {
            archive(nodes_);
        }
    };

    struct Skeleton
    {
        struct Bone
        {
            uint64_t uniqueId_{ 0 };
            std::string name_;
            int64_t parentIndex_{ -1 };
            int64_t nodeIndex_{ 0 };

            DirectX::XMFLOAT4X4 offsetTransform_{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

            bool isOrphan() const { return parentIndex_ < 0; }

            template<class T>
            void serialize(T& archive)
            {
                archive(uniqueId_, name_, parentIndex_, nodeIndex_, offsetTransform_);
            }
        };

        std::vector<Bone> bones_;
        int64_t indexof(uint64_t uniqueId) const
        {
            int64_t index{ 0 };
            for (const Bone& bone : bones_)
            {
                if (bone.uniqueId_ == uniqueId)
                {
                    return index;
                }
                ++index;
            }
            return -1;
        }

        template<class T>
        void serialize(T& archive)
        {
            archive(bones_);
        }
    };

    struct Animation
    {
        std::string name_;
        float samplingRate_{ 0 };

        struct Keyframe
        {
            struct Node
            {
                //ノードのローカル空間からシーンのグローバル空間に変更するために使う
                DirectX::XMFLOAT4X4 globalTransform_{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

                DirectX::XMFLOAT3 scaling_{1, 1, 1};
                DirectX::XMFLOAT4 rotation_{0, 0, 0, 1};//Rotation Quaternion
                DirectX::XMFLOAT3 translation_{0, 0, 0};

                template<class T>
                void serialize(T& archive)
                {
                    archive(globalTransform_, scaling_, rotation_, translation_);
                }
            };
            std::vector<Node> nodes_;

            template<class T>
            void serialize(T& archive)
            {
                archive(nodes_);
            }
        };
        std::vector<Keyframe> sequence_;

        template<class T>
        void serialize(T& archive)
        {
            archive(name_, samplingRate_, sequence_);
        }
    };

    class FbxMeshData : public Asset
    {
    public:
        static const int MAX_BONE_INFLUENCES{ 4 };
        struct Vertex
        {
            DirectX::XMFLOAT3 position_;
            DirectX::XMFLOAT3 normal_;
            DirectX::XMFLOAT4 tangent_{1, 0, 0, 1};
            DirectX::XMFLOAT2 texcoord_;
            FLOAT boneWeights_[MAX_BONE_INFLUENCES]{ 1,0,0,0 };
            INT boneIndices_[MAX_BONE_INFLUENCES]{};

            template<class T>
            void serialize(T& archive)
            {
                archive(position_, normal_, tangent_, texcoord_, boneWeights_, boneIndices_);
            }
        };
        static const int MAX_BONES{ 256 };
        struct Constants
        {
            DirectX::XMFLOAT4X4 world_;
            DirectX::XMFLOAT4 materialColor_;
            DirectX::XMFLOAT4X4 boneTransforms_[MAX_BONES]{ {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1} };
        };
        struct Mesh
        {
            uint64_t uniqueId_{ 0 };
            std::string name_;
            int64_t nodeIndex_{ 0 };

            std::vector<Vertex> vertices_;
            std::vector<uint32_t> indices_;

            DirectX::XMFLOAT4X4 defaultGlobalTransform_{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

            struct Subset
            {
                uint64_t materialUniqueId_{ 0 };
                std::string materialName_;

                uint32_t startIndexLocation_{ 0 };
                uint32_t indexCount_{ 0 };

                template<class T>
                void serialize(T& archive)
                {
                    archive(materialUniqueId_, materialName_, startIndexLocation_, indexCount_);
                }
            };
            std::vector<Subset> subsets_;

            Skeleton bindPose_;

            DirectX::XMFLOAT3 boundingBox_[2]
            {
                {+D3D11_FLOAT32_MAX,+D3D11_FLOAT32_MAX,+D3D11_FLOAT32_MAX},
                {-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
            };

            template<class T>
            void serialize(T& archive)
            {
                archive(uniqueId_, name_, nodeIndex_, subsets_, defaultGlobalTransform_,
                    bindPose_, boundingBox_, vertices_, indices_);
            }

        private:
            Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
            Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer_;
            friend class FbxMeshData;
        };
        std::vector<Mesh> meshes_;

        struct Material
        {
            uint64_t uniqueId_{ 0 };
            std::string name_;

            DirectX::XMFLOAT4 Ka_{0.2f, 0.2f, 0.2f, 1.0f};
            DirectX::XMFLOAT4 Kd_{0.8f, 0.8f, 0.8f, 1.0f};
            DirectX::XMFLOAT4 Ks_{1.0f, 1.0f, 1.0f, 1.0f};

            std::string textureFilenames_[4];
            std::shared_ptr<Texture> textures_[4];

            template<class T>
            void serialize(T& archive)
            {
                archive(uniqueId_, name_, Ka_, Kd_, Ks_, textureFilenames_);
            }
        };
        std::unordered_map<uint64_t, Material> materials_;

        std::vector<Animation> animationClips_;

    private:
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;

        DirectX::XMFLOAT3 position_{ 0, 0, 0 };
        DirectX::XMFLOAT3 angle_{ 0, 0, 0 };
        DirectX::XMFLOAT3 scale_{ 1, 1, 1 };
        DirectX::XMFLOAT4 color_{ 1, 1, 1, 1 };

    public:
        FbxMeshData(ID3D11Device* device, const char* fbxFilename, bool triangulate = false, float samplingRate = 0);
        virtual ~FbxMeshData() = default;

        void CreateComObjects(ID3D11Device* device, const char* fbxFilename);

        //描画エンジンの課題で作成した関数　この中で各シェーダーのセットをしている
        void Render(ID3D11DeviceContext* immediateContext, const Animation::Keyframe* keyframe);
        void Render(ID3D11DeviceContext* immediateContext,
            const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,
            const Animation::Keyframe* keyframe);

        void UpdateAnimation(Animation::Keyframe& keyframe);

        bool AppendAnimations(const char* animationFilename, float samplingRate);

        void BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe);

    protected:
        ModelScene sceneView_;
        float scaleFactor_ = 1.0f;

        void FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes);
        void FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);
        void FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose);
        void FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate);
    };
}

namespace DirectX
{
    //serializeテンプレート関数
    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
            cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
            cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
            cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
            cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

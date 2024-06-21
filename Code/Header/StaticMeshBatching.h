#pragma once
#define NOMINMAX

#include <d3d11.h>
#include <wrl.h>
#include "MathHelper.h"

#include <vector>
#include <unordered_map>

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tinygltf-release/tiny_gltf.h>

#include "Asset.h"

namespace AbyssEngine
{
    class StaticMeshBatching : public Asset
    {
        std::string filename_;
    public:
        StaticMeshBatching(const std::string& filename);
        //virtual ~StaticMeshBaching() = delete;

        struct Scene
        {
            std::string name_;
            std::vector<int> nodes_;
        };
        std::vector<Scene> scenes_;

        struct Node
        {
            std::string name_;
            int skin_ = -1;
            int model_ = -1;

            std::vector<int> children_;

            Vector4 rotation_       = { 0, 0, 0, 1 };
            Vector3 scale_          = {1, 1, 1};
            Vector3 translation_    = {0, 0, 0};

            Matrix globalTransform_ = {
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1 
            };
        };
        std::vector<Node> nodes_;

        struct BufferView
        {
            DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
            Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
            size_t strideInBytes_ = 0;
            size_t sizeInBytes_ = 0;
            size_t Count() const
            {
                return sizeInBytes_ / strideInBytes_;
            }
            //BufferView() = delete;
        };

        struct  Primitive
        {
            int material_;
            std::map<std::string, BufferView> vertexBufferViews_;
            BufferView indexBufferView_;
        };
        std::vector<Primitive> primitives_;

        void Render(const Matrix& world);

        struct TextureInfo
        {
            int index_ = -1;
            int texcoord_ = 0;
        };

        // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo
        struct NormalTextureInfo
        {
            int index_ = -1;
            int texcoord_ = 0;
            int scale_ = 1;
        };

        // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-occlusiontextureinfo
        struct OcclusionTextureInfo
        {
            int index_ = -1;
            int texcoord_ = 0;
            float strength_ = 1;
        };

        //https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
        struct PbrMetallicRoughness
        {
            float basecolorFactor_[4] = { 1,1,1,1 };
            TextureInfo basecolorTexture_;
            float metallicFactor_ = 1;
            float roughnessFactor_ = 1;
            TextureInfo metallicRoughnessTexture_;
        };

        struct Material
        {
            std::string name_;
            struct CBuffer
            {
                float emissiveFactor_[3] = { 0,0,0 };
                int alphaMode_ = 0;
                float alphaCutoff_ = 0.5f;
                int doubleSided_ = 0;

                PbrMetallicRoughness pbrMetallicRoughness_;

                NormalTextureInfo normalTexture_;
                OcclusionTextureInfo occlusionTexture_;
                TextureInfo emissiveTexture_;
            };
            CBuffer data_;
        };
        std::vector<Material> materials_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> materialResourceView_;

        struct Texture
        {
            std::string name_;
            int source_ = -1;
        };
        std::vector<Texture> textures_;
        struct Image
        {
            std::string name_;
            int width_ = -1;
            int height_ = -1;
            int component_ = -1;
            int bits_ = -1;
            int pixelType_ = -1;
            int bufferView_;
            std::string mimeType_;
            std::string uri_;

            bool asIs_ = false;
        };
        std::vector<Image> images_;
        std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureResourceViews_;

        private:
            void FetchNodes(const tinygltf::Model& gltfModel);
            void CumulateTransforms(std::vector<Node>& nodes_);
            BufferView MakeBufferView(const tinygltf::Accessor& accessor);
            void FetchMeshes(const tinygltf::Model& gltfModel);

            Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
            struct PrimitiveConstants
            {
                Matrix world_;
                int material_ = -1;
                int hasTangent_ = 0;
                int pad_[2];
            };
            Microsoft::WRL::ComPtr<ID3D11Buffer> primitiveCBuffer_;

            void FetchMaterials(const tinygltf::Model& gltfModel);
            void FetchTextures(const tinygltf::Model& gltfModel);
    };
}
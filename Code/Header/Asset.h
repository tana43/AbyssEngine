#pragma once
#include <memory>
#include <string>

namespace AbyssEngine
{
    //全てのオブジェクトの基底クラス
    class Asset : public std::enable_shared_from_this<Asset>
    {
    public:
        std::string name_;

    public:
        Asset();
        Asset(const Asset& obj);
        virtual ~Asset();

        [[nodiscard]] std::string ToString() const { return name_; }//名前を返す
        [[nodiscard]] std::string GetInstanceId() const { return instanceId_; }
        

    private:
        std::string instanceId_;//インスタンス時に生成されるユニークID
    };
}


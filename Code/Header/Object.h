#pragma once
#include <memory>
#include <string>

namespace AbyssEngine
{
    //全てのオブジェクトの基底クラス
    class Object : public std::enable_shared_from_this<Object>
    {
    public:
        std::string name_;

    public:
        Object();
        Object(const Object& obj);
        virtual ~Object();

        [[nodiscard]] std::string ToString() const { return name_; }//名前を返す
        [[nodiscard]] std::string GetInstanceId() const { return instanceId_; }
        

    private:
        std::string instanceId_;//インスタンス時に生成されるユニークID
    };
}


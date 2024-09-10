#include "Transform.h"
#include "Component.h"

#include "imgui/imgui.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace AbyssEngine;
using namespace std;

void Transform::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = std::static_pointer_cast<Transform>(shared_from_this());

    //初期位置を保存し、読み込み
    //Jsonファイル読み込み、書き出し
    {
        //ファイルの読み込み
        bool exist = true;
        nlohmann::json mJson = actor_->ReadingJsonFile(exist);

        //ファイルが存在している場合のみ読み込み
        if(exist)
        {
            if (mJson.find("Transform") != mJson.end())
            {
                //読み込んだデータをそれぞれの変数に代入する
                auto& data = mJson["Transform"];
                position_ = { data["Position"][0],data["Position"][1],data["Position"][2] };
                rotation_ = { data["Rotation"][0], data["Rotation"][1], data["Rotation"][2] };
                scale_ = { data["Scale"][0], data["Scale"][1], data["Scale"][2] };
                scaleFactor_ = data["ScaleFactor"];

                localPosition_ = { data["L_Position"][0],data["L_Position"][1],data["L_Position"][2] };
                localRotation_ = { data["L_Rotation"][0], data["L_Rotation"][1], data["L_Rotation"][2] };
                localScale_ = { data["L_Scale"][0], data["L_Scale"][1], data["L_Scale"][2] };
                localScaleFactor_ = data["L_ScaleFactor"];
            }
            else
            {
                //データが見つからなかったので作成
                mJson["Transform"] = {
                    {"Position",{0.0f,0.0f,0.0f}},
                    {"Rotation",{0.0f,0.0f,0.0f}},
                    {"Scale",{1.0f,1.0f,1.0f}},
                    {"ScaleFactor",1.0f},

                    {"L_Position",{0.0f,0.0f,0.0f}},
                    {"L_Rotation",{0.0f,0.0f,0.0f}},
                    {"L_Scale",{1.0f,1.0f,1.0f}},
                    {"L_ScaleFactor",1.0f}
                };
            }

            //ファイルに内容を書き込む
            actor_->WritingJsonFile(mJson);
        }
    }
}

Matrix Transform::CalcWorldMatrix()
{
    Matrix S, R, T;

    //親子関係の有無
    if (const auto& parent = actor_->GetParent().lock())
    {
        const Matrix localMatrix = CalcLocalMatrix();
        const auto scale = scale_ * scaleFactor_;
        worldMatrix_ = localMatrix * parent->GetTransform()->CalcWorldMatrix();
        worldMatrix_.Decompose(scale_, rotation_, position_);
        R = Matrix::CreateFromQuaternion(rotation_);
    }
    else
    {
        //オイラー角をクォータニオンに変換
        Vector3 euler = { rotation_.x,rotation_.y,rotation_.z };
        const auto scale = scale_ * scaleFactor_;
        S = Matrix::CreateScale(scale);
        R = Matrix::CreateFromQuaternion(Quaternion::Euler(euler));
        T = Matrix::CreateTranslation(position_);
        worldMatrix_ = S * R * T;
    }
    
    //各方向ベクトルの算出
    forward_ = Vector3::Transform(Vector3::Forward, R);
    forward_.Normalize();

    right_ = Vector3::Transform(Vector3::Right, R);
    right_.Normalize();

    up_ = Vector3::Transform(Vector3::Up, R);
    up_.Normalize();

    return worldMatrix_;
}

Matrix Transform::CalcLocalMatrix()
{
    //オイラー角をクォータニオンに変換
    Vector3 euler = { localRotation_.x,localRotation_.y,localRotation_.z };
    Matrix localS = Matrix::CreateScale(localScale_ * localScaleFactor_);
    Matrix localR = Matrix::CreateFromQuaternion(Quaternion::Euler(euler));
    Matrix localT = Matrix::CreateTranslation(localPosition_);
    localMatrix_ = localS * localR * localT;
    return localMatrix_;
}

Vector3 Transform::GetEulerAngles() const 
{
    return rotation_.To_Euler();
}

void Transform::DrawImGui()
{
#if _DEBUG
    if (ImGui::TreeNode("Transform"))
    {
        //親がいないならぐワールド座標等を表示
        if (!actor_->GetParent().lock())
        {
            ImGui::DragFloat3("Position", &position_.x, 0.1f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat3("Scale", &scale_.x, 0.01f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat3("Rotation", &rotation_.x, 0.5f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat("ScaleFactor", &scaleFactor_, 0.01f, 0.01f, 100.0f);

            //セーブ
            //if (ImGui::ButtonDoubleChecking("Save",doubleCheckFlag_))
            //{
            //    //ファイルの読み込み
            //    nlohmann::json mJson = actor_->ReadingJsonFile();

            //    auto& data = mJson["Transform"];
            //    data["Position"] = { position_.x,position_.y,position_.z };
            //    data["Rotation"] = { rotation_.x,rotation_.y,rotation_.z };
            //    data["Scale"] = { scale_.x,scale_.y,scale_.z };
            //    data["ScaleFactor"] = scaleFactor_;

            //    //ファイルに内容を書き込む
            //    actor_->WritingJsonFile(mJson);
            //}
        }
        else //親がいないならローカルを表示
        {
            ImGui::Text("-----Local------");
            ImGui::DragFloat3("LPosition", &localPosition_.x, 0.1f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat3("LScale", &localScale_.x, 0.01f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat3("LRotation", &localRotation_.x, 0.5f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat("LScaleFactor", &localScaleFactor_, 0.01f, 0.01f, 100.0f);

            //セーブ
            //if (ImGui::ButtonDoubleChecking("Local Save", doubleCheckFlag_))
            //{
            //    //ファイルの読み込み
            //    nlohmann::json mJson = actor_->ReadingJsonFile();

            //    auto& data = mJson["Transform"];
            //    data["L_Position"] = { localPosition_.x,localPosition_.y,localPosition_.z };
            //    data["L_Rotation"] = { localRotation_.x,localRotation_.y,localRotation_.z };
            //    data["L_Scale"] = { localScale_.x,localScale_.y,localScale_.z };
            //    data["L_ScaleFactor"] = localScaleFactor_;

            //    //ファイルに内容を書き込む
            //    actor_->WritingJsonFile(mJson);
            //}
        }

        //Jsonへ保存
        if (ImGui::ButtonDoubleChecking("Save", doubleCheckFlag_))
        {
            SaveToJson();
        }

        ImGui::TreePop();
    }

#endif // _DEBUG
}

void AbyssEngine::Transform::SaveToJson()
{
    //ファイルの読み込み
    nlohmann::json mJson = actor_->ReadAndCreateJsonFile();

    auto& data = mJson["Transform"];
    data["Position"] = { position_.x,position_.y,position_.z };
    data["Rotation"] = { rotation_.x,rotation_.y,rotation_.z };
    data["Scale"] = { scale_.x,scale_.y,scale_.z };
    data["ScaleFactor"] = scaleFactor_;

    data["L_Position"] = { localPosition_.x,localPosition_.y,localPosition_.z };
    data["L_Rotation"] = { localRotation_.x,localRotation_.y,localRotation_.z };
    data["L_Scale"] = { localScale_.x,localScale_.y,localScale_.z };
    data["L_ScaleFactor"] = localScaleFactor_;

    //ファイルに内容を書き込む
    actor_->WritingJsonFile(mJson);
}

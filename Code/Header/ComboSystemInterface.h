#pragma once

//ComboSystem周りで使用する定数を定義してます
class ComboSystemInterface
{
public:

    //どのスティック入力に対応したコンボか
    //格闘ボタン + スティック入力方向
    enum class InputDirection
    {
        Null, //ルートなどの判定しない物に付ける
        Neutral, //ニュートラル
        Front, //前
        Side, //横
        Back, //後ろ
    };

    //実行情報
    enum class State
    {
        NotCancellation,  //キャンセルできない
        Cancellation,     //次のコンボへ派生もしくは回避キャンセル可能
        Complete,         //派生することなく完了
    };
};
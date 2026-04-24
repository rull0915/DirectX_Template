//====================================================//
// ファイル名   : HitContact.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/24
//
// 概要 : 衝突情報をまとめた構造体
//
// 更新履歴 :
// 2026/04/24 新規作成
//====================================================//

#pragma once

class GameObject;

//====================================================//
// 構造体宣言
//====================================================//

struct ObjectPair
{
    GameObject* a;
    GameObject* b;

public:
    bool operator==(ObjectPair& other)
    {
        return (a == other.a && b == other.b) ||
            (a == other.b && b == other.a);
    }
};

struct HitContact
{
    GameObject* a;
    GameObject* b;

    bool isTrigger;

    DirectX::SimpleMath::Vector3 normal;
    float penetration;

    DirectX::SimpleMath::Vector3 point;

    ObjectPair pair;
};
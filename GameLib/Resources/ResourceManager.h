//====================================================//
// ファイル名   : ResourceManager.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/31
//
// 概要 :
//
// 更新履歴 : リソース関連をまとめたヘッダ
// 2026/03/31 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <unordered_map>
#include <string>

#include <SpriteFont.h>
#include <Model.h>
#include <Effects.h>

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class ResourceManager
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // デバイス
    ID3D11Device1* m_device;

    // エフェクト
    std::unique_ptr<DirectX::EffectFactory> m_effect;

    // 画像リソース
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;

    // フォント
    std::unordered_map<std::string, std::unique_ptr<DirectX::SpriteFont>> m_spriteFonts;

    // モデル
    std::unordered_map<std::string, std::unique_ptr<DirectX::Model>> m_models;

private:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    ResourceManager();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------
public:

    ~ResourceManager();

    // シングルトン化
    static ResourceManager& Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    // 初期化
    void Initialize(ID3D11Device1* device);

    // 画像の追加、読み込み
    void AddTexture(const std::string& kayName, const std::wstring& filePass);

    // フォントの追加、読み込み
    void AddFont(const std::string& key, const std::wstring& filePath);

    // モデルの追加、読み込み
    void AddModel(const std::string& key, const std::wstring& filePath);

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    ID3D11ShaderResourceView* GetTexture(const std::string& key) const;

    DirectX::SpriteFont* GetSpriteFont(const std::string& key) const;

    DirectX::Model* GetModel(const std::string& key) const;

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};
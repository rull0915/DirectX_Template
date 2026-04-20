//====================================================//
// ファイル名  : ResourceManager.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/03/31
//
// 概要        :  リソース管理クラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "ResourceManager.h"

#include <WICTextureLoader.h>

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//


ResourceManager::ResourceManager()
    : m_device{ nullptr }
    , m_effect{}
    , m_textures{}
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Initialize(ID3D11Device1* device)
{
    m_device = device;

    m_effect = std::make_unique<EffectFactory>(device);
    m_effect->SetDirectory(L"Resources/Models");
}

void ResourceManager::AddTexture(const std::string& keyName, const std::wstring& filePass)
{
    // キーの重複チェック
    if (m_textures.find(keyName) != m_textures.end()) return;

    // テクスチャを用意
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

    HRESULT hr = CreateWICTextureFromFile(
        m_device,                 // ID3D11Device*
        filePass.data(),          // ファイルパス
        nullptr,
        texture.GetAddressOf()
    );

    if (FAILED(hr))
    {
    }

    m_textures.insert(std::make_pair(keyName, texture));
}

void ResourceManager::AddFont(const std::string& key, const std::wstring& filePath)
{
    // 既に同じキーが存在する場合はスキップ
    if (m_spriteFonts.find(key) != m_spriteFonts.end()) return;

    // 読み込み
    m_spriteFonts.insert(std::make_pair(key, std::make_unique<SpriteFont>(m_device, filePath.c_str())));
}

void ResourceManager::AddModel(const std::string& key, const std::wstring& filePath)
{
    // 既に同じキーが存在する場合はスキップ
    if (m_models.find(key) != m_models.end()) return;

    auto playerModel = Model::CreateFromCMO(m_device, filePath.c_str(), *m_effect);

    m_models.insert(std::make_pair(key, std::move(playerModel)));
}

ID3D11ShaderResourceView* ResourceManager::GetTexture(const std::string& key) const
{
    // 存在しないキーなら
    if (m_textures.find(key) == m_textures.end()) return nullptr;

    return m_textures.at(key).Get();
}

SpriteFont* ResourceManager::GetSpriteFont(const std::string& key) const
{
    // 存在しないキーなら
    if (m_spriteFonts.find(key) == m_spriteFonts.end()) return nullptr;

    return m_spriteFonts.at(key).get();
}

DirectX::Model* ResourceManager::GetModel(const std::string& key) const
{
    // 存在しないキーなら
    if (m_models.find(key) == m_models.end()) return nullptr;

    return m_models.at(key).get();
}

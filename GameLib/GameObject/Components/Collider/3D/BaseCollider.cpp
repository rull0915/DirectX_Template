//====================================================//
// ファイル名  : BaseCollider.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/02
//
// 概要        :  コライダーの基底クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "BaseCollider.h"

#include "GameLib/GameObject/Managers/3DManagers/Collider/CollideManager.h"

#include "GameLib/GameObject/GameObject.h"
#include "GameLib/MyRenderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
BaseCollider::BaseCollider(GameObject* own, ColliderType type, int ID, bool isMain, DirectX::SimpleMath::Vector3 center)
	: ICollider(own, ID, isMain)
	, m_type{ type }
	, m_boundingBox{ {0, 0, 0}, {0, 0, 0} }
	, m_localCenterPos{ center }
{
	// 自身をマネージャーに登録
	CollideManager::Instance().AddCollide(this);
}

BaseCollider::~BaseCollider()
{
}

void AABB::DebugDraw(Renderer& renderer, int color) const
{
	// --- 色の展開 ---
	float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0xFF) / 255.0f;

	DirectX::SimpleMath::Vector4 col(r, g, b, 1.0f);

	DirectX::VertexPositionColor vertexes[8]
	{
		{ {min.x, min.y, min.z}, col},
		{ {min.x, min.y, max.z}, col},
		{ {min.x, max.y, max.z}, col},
		{ {min.x, max.y, min.z}, col},
		{ {max.x, min.y, min.z}, col},
		{ {max.x, min.y, max.z}, col},
		{ {max.x, max.y, max.z}, col},
		{ {max.x, max.y, min.z}, col},
	};

	uint16_t indexes[]
	{
		0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7
	};

	renderer.Draw().PrimitiveIndex(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, indexes, 24, vertexes, 8);
}

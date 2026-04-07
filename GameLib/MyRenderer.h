#pragma once

// Normal
#include <map>
#include <string>

// DirectXTK
#include <CommonStates.h>
#include <PrimitiveBatch.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <Effects.h>
#include <Model.h>

// 自作描画用クラス
class MyRenderer
{
	// ===== メンバ変数宣言 ===== //
private:

	// 実体
	static MyRenderer m_instance;

	// 初期化済みフラグ
	bool m_initialized;

	// 描画開始済みフラグ
	bool m_isDrawStarted;

	// デバイス
	ID3D11Device* m_device;

	// コンテキスト
	ID3D11DeviceContext* m_nowContext;

	// 適用中のView
	DirectX::SimpleMath::Matrix m_view;
	// 適用中のProj
	DirectX::SimpleMath::Matrix m_proj;

	// 現在の透明度
	float m_alphaValue;

	enum class DrawMode
	{
		None,
		UI,
		Primitiv,
		Sprite,
		Font,
		Model
	};

	DrawMode m_nowMode;

	// ======= PrimitvBatch関連 ======== //

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;	// プリミティブバッチ

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;	// ベーシックエフェクト

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// コモンステート
	DirectX::CommonStates* m_states;

	// 行列が変更されたかどうか
	bool m_isChangeWVP;

	// ======= SpriteBatch関連 ========= //
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// ===== メンバ関数宣言 ===== //
private:
	// コンストラクタ
	MyRenderer(
	);

public:
	// 初期化関数
	static void Initialize(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		DirectX::CommonStates* state
	);

	// 描画を開始する関数
	static void StartDraw(ID3D11DeviceContext* context);

	// 描画を終了する関数
	static void EndDraw();

	// ========= PrimitivBatch関連関数 ============ //

	static void SetWorld(DirectX::SimpleMath::Matrix world);
	static void SetView(DirectX::SimpleMath::Matrix view);
	static void SetProjection(DirectX::SimpleMath::Matrix proj);
	static void ApplySetting();

	// ================ 描画関数 ================== //
#pragma region PrimitivBatch

	static void DrawTriangle	// 三角形描画関数 
	(
		DirectX::VertexPositionColor v1,	// 頂点情報
		DirectX::VertexPositionColor v2,	// 頂点情報
		DirectX::VertexPositionColor v3		// 頂点情報
	);
	static void DrawTriangle	// 短縮版
	(
		DirectX::SimpleMath::Vector3 p1,
		DirectX::SimpleMath::Vector3 p2,
		DirectX::SimpleMath::Vector3 p3,
		int color
	);
	static void DrawTriangle2D	// 短縮版
	(
		DirectX::SimpleMath::Vector2 p1,
		DirectX::SimpleMath::Vector2 p2,
		DirectX::SimpleMath::Vector2 p3,
		int color
	);

	static void DrawRect
	(
		DirectX::VertexPositionColor v1,	// 頂点情報
		DirectX::VertexPositionColor v2,	// 頂点情報
		DirectX::VertexPositionColor v3,	// 頂点情報
		DirectX::VertexPositionColor v4		// 頂点情報
	);

	static void DrawRect	// 短縮版
	(
		DirectX::SimpleMath::Vector3 p1,
		DirectX::SimpleMath::Vector3 p2,
		DirectX::SimpleMath::Vector3 p3,
		DirectX::SimpleMath::Vector3 p4,
		int color
	);

	static void DrawLine
	(
		DirectX::VertexPositionColor v1,	// 頂点情報
		DirectX::VertexPositionColor v2		// 頂点情報
	);

	static void DrawLine
	(
		DirectX::SimpleMath::Vector3 start,
		DirectX::SimpleMath::Vector3 end,
		int color
	);

	static void DrawCircle
	(
		DirectX::SimpleMath::Vector3 centerPos,
		DirectX::SimpleMath::Vector3 normal,
		float radius,
		int division,
		int color,
		bool fillFrag
	);

	// 補助関数：2つの方向ベクトル間の弧を描画する
	static void DrawArc(
		const DirectX::SimpleMath::Vector3& center,
		DirectX::SimpleMath::Vector3 vStart, // 開始方向（正規化されている前提）
		DirectX::SimpleMath::Vector3 vEnd,   // 終了方向（正規化されている前提）
		float radius,
		int color,
		int segments = 16);

	//  ----- UI ----- //
	static void DrawBox
	(
		DirectX::SimpleMath::Vector2 posA,
		DirectX::SimpleMath::Vector2 posB,
		int color
	);

	// ----- 3D ----- //
	static void Draw3DBox
	(
		DirectX::SimpleMath::Vector3 posA,
		DirectX::SimpleMath::Vector3 posB,
		int color,
		bool fillFrag = false
	);

#pragma endregion

#pragma region SpriteBatch

	static void DrawGraph
	(
		DirectX::SimpleMath::Vector2 pos,
		ID3D11ShaderResourceView* texture
	);

	static void DrawTurnGraph
	(
		DirectX::SimpleMath::Vector2 pos,
		ID3D11ShaderResourceView* texture
	);

	static void DrawExtendGraph
	(
		DirectX::SimpleMath::Vector2 min,
		DirectX::SimpleMath::Vector2 max,
		ID3D11ShaderResourceView* texture
	);

#pragma endregion

#pragma region SpriteFont

	static void DrawString
	(
		DirectX::SpriteFont* font,
		const wchar_t* text, 
		DirectX::SimpleMath::Vector2 pos, 
		int color = 0xFFFFFF
	);

	static void DrawStringOnCenter
	(
		DirectX::SpriteFont* font,
		const wchar_t* text,
		DirectX::SimpleMath::Vector2 pos,
		int color = 0xFFFFFF
	);

	static void DrawExtendString
	(
		DirectX::SpriteFont* font,
		const wchar_t* text,
		DirectX::SimpleMath::Vector2 pos,
		float ratio,
		int color = 0xFFFFFF
	);

	static void DrawExtendStringOnCenter
	(
		DirectX::SpriteFont* font,
		const wchar_t* text,
		DirectX::SimpleMath::Vector2 pos,
		float ratio,
		int color = 0xFFFFFF
	);

#pragma endregion

#pragma region Model

	static void DrawModel
	(
		DirectX::Model* model, 
		DirectX::SimpleMath::Matrix world
	);

	static void DrawModel
	(
		DirectX::Model* model,
		DirectX::SimpleMath::Vector3 pos, // 位置
		DirectX::SimpleMath::Vector3 rot = { 0, 0, 0 },	// 回転
		float scale = 1.0f	// 大きさ
	);

#pragma endregion

	// =============== その他関数 ================= //
private:
	// 変更を確認する関数
	static void CheckChange(DrawMode next);

	// テクスチャのサイズを取得する関数
	static DirectX::SimpleMath::Vector2 GetTextureSize(ID3D11ShaderResourceView* tex);

public:
	// α値を変更する関数
	static void SetAlpha(float ratio) 
	{
		m_instance.m_alphaValue = ratio; 
	}
};


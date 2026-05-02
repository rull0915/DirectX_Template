//====================================================//
// ファイル名  : PrimitiveRenderer.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/30
//
// 概要        :  プリミティブ描画クラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "PrimitiveRenderer.h"
#include "../../Renderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

#pragma region Setting

/// <summary>
/// コンストラクタ
/// </summary>
PrimitiveRenderer::PrimitiveRenderer(Renderer& renderer)
	: m_renderer{ renderer }
	, m_primitiveBatch{}
	, m_basicEffect{}
	, m_inputLayout{}
	, m_renderState{ renderer.GetRenderState() }
	, m_isStarted{ false }
{
}

/// <summary>
/// 初期化関数
/// </summary>
void PrimitiveRenderer::Initialize()
{
	ID3D11Device* device = m_renderer.GetDevice();
	ID3D11DeviceContext* context = m_renderer.GetContext();

	// プリミティブバッチの初期化
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);

	// ベーシックエフェクトの初期化
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);

	// 設定
	m_basicEffect->SetVertexColorEnabled(true);	// 頂点カラーの使用をオン
	m_basicEffect->SetLightingEnabled(false);	// ライトの使用をオフ
	m_basicEffect->SetTextureEnabled(false);	// テクスチャの使用をオフ

	// インプットレイアウトの作成
	// 例外処理
	DX::ThrowIfFailed(
		DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf()));

	// WVP行列の初期化
	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);			// ワールド
	m_basicEffect->SetView(DirectX::SimpleMath::Matrix::Identity);			// ビュー
	m_basicEffect->SetProjection(DirectX::SimpleMath::Matrix::Identity);	// プロジェクション
}

/// <summary>
/// 描画を開始する関数
/// </summary>
/// <param name="context"></param>
/// <param name="states"></param>
void PrimitiveRenderer::Start()
{
	// 描画開始済みならスキップ
	if (m_isStarted) return;

	// フラグをセット
	m_isStarted = true;

	// 各項目の設定
	ID3D11DeviceContext* context = m_renderer.GetContext();
	DirectX::CommonStates* states = m_renderer.GetStates();

	// 透明度を使用
	context->OMSetBlendState(states->NonPremultiplied(), nullptr, 0xFFFFFFFF);

	// デプスソートを使用
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	// カリング不使用
	context->RSSetState(states->CullNone());

	// インプットレイアウトをセット
	context->IASetInputLayout(m_inputLayout.Get());

	// サンプラーをリセット
	ID3D11SamplerState* samplers[] = { nullptr };
	context->PSSetSamplers(0, 1, samplers);

	// エフェクトを適用
	m_basicEffect->Apply(context);

	// 描画の開始
	m_primitiveBatch->Begin();
}

void PrimitiveRenderer::End()
{
	// 開始済みでなければスキップ
	if (!m_isStarted) return;

	// 描画終了
	m_primitiveBatch->End();

	// 描画開始フラグをオフに
	m_isStarted = false;
}

/// <summary>
/// ステートの変更を調べる関数
/// </summary>
void PrimitiveRenderer::CheckChangeState()
{
	// 値が変更されていたら
	if (m_renderState.IsDirty(RenderType::PRIMITIVE, MatrixType::ALL))
	{
		// 描画を一旦終了
		End();

		// 各行列のセット
		if (m_renderState.IsDirty(RenderType::PRIMITIVE, MatrixType::WORLD)) m_basicEffect->SetWorld(m_renderState.GetWorld());
		if (m_renderState.IsDirty(RenderType::PRIMITIVE, MatrixType::VIEW))  m_basicEffect->SetView(m_renderState.GetView());
		if (m_renderState.IsDirty(RenderType::PRIMITIVE, MatrixType::PROJECTION)) m_basicEffect->SetProjection(m_renderState.GetProjection());

		// フラグリセット
		m_renderState.ResetFlag(RenderType::PRIMITIVE, MatrixType::ALL);

		// 描画の再開
		Start();
	}
}

#pragma endregion

#pragma region Draw

/// <summary>
/// 三角形描画関数
/// </summary>
/// <param name="v1">頂点1</param>
/// <param name="v2">頂点2</param>
/// <param name="v3">頂点3</param>
void PrimitiveRenderer::DrawTriangle(DirectX::VertexPositionColor v1, DirectX::VertexPositionColor v2, DirectX::VertexPositionColor v3, bool fillFlag)
{
	// ステート変更のチェック
	CheckChangeState();

	DirectX::VertexPositionColor vertexes[3] = { v1, v2, v3 };

	// 描画
	 
	// 塗りつぶすなら
	if (fillFlag)
	{
		m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexes, 3);
	}
	else
	{
		uint16_t indexes[] = { 0, 1, 1, 2, 2, 0 };
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, indexes, 6, vertexes, 3);
	}
}

/// <summary>
/// 三角形描画関数
/// </summary>
/// <param name="p1">頂点座標1</param>
/// <param name="p2">頂点座標2</param>
/// <param name="p3">頂点座標3</param>
/// <param name="color">色</param>
void PrimitiveRenderer::DrawTriangle(DirectX::SimpleMath::Vector3 p1, DirectX::SimpleMath::Vector3 p2, DirectX::SimpleMath::Vector3 p3, int color, bool fillFlag)
{
	// ステート変更のチェック
	CheckChangeState();

	float r = static_cast<float>((color >> 16) & 0b11111111) / 0xFF;
	float g = static_cast<float>((color >> 8) & 0b11111111) / 0xFF;
	float b = static_cast<float>((color >> 0) & 0b11111111) / 0xFF;
	float a = m_renderState.GetAlpha();

	DirectX::VertexPositionColor v1(p1, { r, g, b, a });
	DirectX::VertexPositionColor v2(p2, { r, g, b, a });
	DirectX::VertexPositionColor v3(p3, { r, g, b, a });

	DirectX::VertexPositionColor vertexes[3] = { v1, v2, v3 };

	// 描画

	// 塗りつぶすなら
	if (fillFlag)
	{
		m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexes, 3);
	}
	else
	{
		uint16_t indexes[] = { 0, 1, 1, 2, 2, 0 };
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, indexes, 6, vertexes, 3);
	}
}

/// <summary>
/// 矩形描画関数
/// </summary>
/// <param name="v1">頂点情報1</param>
/// <param name="v2">頂点情報2</param>
/// <param name="v3">頂点情報3</param>
/// <param name="v4">頂点情報4</param>
void PrimitiveRenderer::DrawRect(DirectX::VertexPositionColor v1, DirectX::VertexPositionColor v2, DirectX::VertexPositionColor v3, DirectX::VertexPositionColor v4, bool fillFlag)
{
	CheckChangeState();

	DirectX::VertexPositionColor vertexes[4] = { v1, v2, v3, v4 };

	// 描画

	// 塗りつぶすなら
	if (fillFlag)
	{
		uint16_t indexes[] = { 0, 1, 2, 2, 3, 0 };
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, 6, vertexes, 4);
	}
	else
	{
		uint16_t indexes[] = { 0, 1, 1, 2, 2, 3, 3, 0 };
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, indexes, 8, vertexes, 4);
	}
}

/// <summary>
/// 矩形描画関数
/// </summary>
/// <param name="p1">頂点座標1</param>
/// <param name="p2">頂点座標2</param>
/// <param name="p3">頂点座標3</param>
/// <param name="p4">頂点座標4</param>
/// <param name="color">色</param>
void PrimitiveRenderer::DrawRect(DirectX::SimpleMath::Vector3 p1, DirectX::SimpleMath::Vector3 p2, DirectX::SimpleMath::Vector3 p3, DirectX::SimpleMath::Vector3 p4, int color, bool fillFlag)
{
	CheckChangeState();

	float r = static_cast<float>((color >> 16) & 0b11111111) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0b11111111) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0b11111111) / 255.0f;
	float a = m_renderState.GetAlpha();

	DirectX::VertexPositionColor v1(p1, { r, g, b, a });
	DirectX::VertexPositionColor v2(p2, { r, g, b, a });
	DirectX::VertexPositionColor v3(p3, { r, g, b, a });
	DirectX::VertexPositionColor v4(p4, { r, g, b, a });

	DirectX::VertexPositionColor vertexes[4] = { v1, v2, v3, v4 };

	// 塗りつぶすなら
	if (fillFlag)
	{
		uint16_t indexes[] = { 0, 1, 2, 2, 3, 0 };
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, 6, vertexes, 4);
	}
	else
	{
		uint16_t indexes[] = { 0, 1, 1, 2, 2, 3, 3, 0 };
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, indexes, 8, vertexes, 4);
	}

}

/// <summary>
/// 線描画関数
/// </summary>
/// <param name="v1">頂点1</param>
/// <param name="v2">頂点2</param>
void PrimitiveRenderer::DrawLine(DirectX::VertexPositionColor v1, DirectX::VertexPositionColor v2)
{
	CheckChangeState();

	m_primitiveBatch->DrawLine(v1, v2);
}

/// <summary>
/// 線描画関数
/// </summary>
/// <param name="start">頂点座標1</param>
/// <param name="end">頂点座標2</param>
/// <param name="color">色</param>
void PrimitiveRenderer::DrawLine(DirectX::SimpleMath::Vector3 start, DirectX::SimpleMath::Vector3 end, int color)
{
	CheckChangeState();

	float r = static_cast<float>((color >> 16) & 0b11111111) / 0xFF;
	float g = static_cast<float>((color >> 8) & 0b11111111) / 0xFF;
	float b = static_cast<float>((color >> 0) & 0b11111111) / 0xFF;
	float a = m_renderState.GetAlpha();

	DirectX::VertexPositionColor v1(start, { r, g, b, a });
	DirectX::VertexPositionColor v2(end, { r, g, b, a });

	m_primitiveBatch->DrawLine(v1, v2);
}

/// <summary>
/// 円描画関数
/// </summary>
/// <param name="centerPos">中心座標</param>
/// <param name="normal">描く面の法線</param>
/// <param name="radius">半径</param>
/// <param name="division">分割数</param>
/// <param name="color">色</param>
void PrimitiveRenderer::DrawCircle(DirectX::SimpleMath::Vector3 centerPos, DirectX::SimpleMath::Vector3 normal, float radius, int division, int color, bool fillFlag)
{
	CheckChangeState();

	if (division < 3) return;

	// --- 色の展開 ---
	float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0xFF) / 255.0f;
	float a = m_renderState.GetAlpha();

	DirectX::SimpleMath::Vector4 col(r, g, b, a);

	// --- 基底ベクトルの計算 ---
	normal.Normalize();
	DirectX::SimpleMath::Vector3 up = (std::abs(normal.y) > 0.9f) ? DirectX::SimpleMath::Vector3::UnitX : DirectX::SimpleMath::Vector3::UnitY;
	DirectX::SimpleMath::Vector3 vU = normal.Cross(up);
	vU.Normalize();
	DirectX::SimpleMath::Vector3 vV = normal.Cross(vU);
	vV.Normalize();

	// 角度の間隔を算出
	float step = 2 * PI_F / static_cast<float>(division);

	// 頂点座標を格納する配列を用意
	std::vector<DirectX::VertexPositionColor> vertexes;
	vertexes.resize(division + 1);

	// 分割数分ループ
	for (int i = 0; i < division; ++i)
	{
		float theta = step * i;

		// 頂点情報を計算
		DirectX::VertexPositionColor v(centerPos + (vU * cosf(theta) + vV * sinf(theta)) * radius, col);

		vertexes[i] = v;
	}

	std::vector<uint16_t> indexes;

	// 塗りつぶすとき
	if (fillFlag)
	{
		vertexes[division] = DirectX::VertexPositionColor(centerPos, col);

		indexes.resize(division * 3);

		for (uint16_t i = 0; i < division; i++)
		{
			indexes[i * 3 + 0] = i;
			indexes[i * 3 + 1] = (i + 1) % division;
			indexes[i * 3 + 2] = division;
		}

		// 三角形を構築して渡す
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes.data(), division * 3, vertexes.data(), division + 1);
	}
	// ワイヤーフレームの時
	else
	{
		indexes.resize(division * 2);

		for (uint16_t i = 0; i < division; i++)
		{
			indexes[i * 2] = i;
			indexes[i * 2 + 1] = (i + 1) % division;
		}

		// 線を構築して渡す
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, indexes.data(), division * 2, vertexes.data(), division);
	}
}

/// <summary>
/// 弧の描画関数
/// </summary>
/// <param name="center">中心座標</param>
/// <param name="vStart">弧の開始位置</param>
/// <param name="vEnd">弧の終了位置</param>
/// <param name="division">分割数</param>
/// <param name="radius">半径</param>
/// <param name="color">色</param>
void PrimitiveRenderer::DrawArc(const DirectX::SimpleMath::Vector3& center, DirectX::SimpleMath::Vector3 vStart, DirectX::SimpleMath::Vector3 vEnd, int division, float radius, int color, bool fillFlag)
{
	CheckChangeState();

	if (radius <= 0.0f || division <= 0) return;

	// ベクトルの正規化（中心からの方向ベクトルとして扱う）
	vStart.Normalize();
	vEnd.Normalize();

	// 平面の法線を求める
	DirectX::SimpleMath::Vector3 normal = vStart.Cross(vEnd);

	// 法線が決まらなかった場合スキップ
	if (normal.LengthSquared() < 0.000001f) {
		return;
	}
	normal.Normalize();

	// 基底ベクトル U, V を作成
	// vStart を角度 0 とする
	DirectX::SimpleMath::Vector3 vU = vStart;
	// U と法線に垂直なベクトルを V とする
	DirectX::SimpleMath::Vector3 vV = normal.Cross(vU);
	vV.Normalize();

	// 終了角を求める
	float endRadian = std::atan2(vV.Dot(vEnd), vU.Dot(vEnd));

	// --- 色の展開 ---
	float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0xFF) / 255.0f;
	float a = m_renderState.GetAlpha();

	DirectX::SimpleMath::Vector4 col(r, g, b, a);

	// 描画
	float step = endRadian / static_cast<float>(division);

	// 頂点座標を格納する配列を用意
	std::vector<DirectX::VertexPositionColor> vertexes;
	vertexes.resize(division + 2);

	for (int i = 0; i <= division; ++i)
	{
		float theta = step * i;

		// 円周上の座標を算出
		DirectX::SimpleMath::Vector3 currentPoint = center + (vU * cosf(theta) + vV * sinf(theta)) * radius;

		DirectX::VertexPositionColor v(currentPoint, col);

		vertexes[i] = v;
	}

	std::vector<uint16_t> indexes;

	// 塗りつぶすとき
	if (fillFlag)
	{
		vertexes[division + 1] = DirectX::VertexPositionColor(center, col);

		indexes.resize((division + 1) * 3);

		for (uint16_t i = 0; i <= division; i++)
		{
			indexes[i * 3 + 0] = i;
			indexes[i * 3 + 1] = (i + 1);
			indexes[i * 3 + 2] = division + 1;
		}

		// 三角形を構築して渡す
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes.data(), division * 3 + 3, vertexes.data(), division + 2);
	}
	// ワイヤーフレームの時
	else
	{
		indexes.resize((division) * 2);

		for (uint16_t i = 0; i < division; i++)
		{
			indexes[i * 2] = i;
			indexes[i * 2 + 1] = (i + 1);
		}

		// 線を構築して渡す
		m_primitiveBatch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, indexes.data(), division * 2 + 2, vertexes.data(), division + 1);
	}
}

/// <summary>
/// 頂点情報を渡して描画する関数
/// </summary>
/// <param name="topology">トポロジータイプ</param>
/// <param name="indexes">インデックスの配列</param>
/// <param name="indexCount">インデックスの数</param>
/// <param name="vertexes">頂点配列</param>
/// <param name="vertexCount">頂点の数</param>
void PrimitiveRenderer::DrawIndex(D3D11_PRIMITIVE_TOPOLOGY topology, uint16_t* indexes, uint16_t indexCount, DirectX::VertexPositionColor* vertexes, uint16_t vertexCount)
{
	m_primitiveBatch->DrawIndexed(topology, indexes, indexCount, vertexes, vertexCount);
}

#pragma endregion

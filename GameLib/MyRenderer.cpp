#include "pch.h"
#include "MyRenderer.h"

#include "Game/Screen.h"

using namespace DirectX;

// 実体の宣言
MyRenderer MyRenderer::m_instance;

const float MyRenderer::SinCache16[16] =
{
	sinf(PI_F * (0.0f / 8)),
	sinf(PI_F * (1.0f / 8)),
	sinf(PI_F * (2.0f / 8)),
	sinf(PI_F * (3.0f / 8)),
	sinf(PI_F * (4.0f / 8)),
	sinf(PI_F * (5.0f / 8)),
	sinf(PI_F * (6.0f / 8)),
	sinf(PI_F * (7.0f / 8)),
	sinf(PI_F * (8.0f / 8)),
	sinf(PI_F * (9.0f / 8)),
	sinf(PI_F * (10.0f / 8)),
	sinf(PI_F * (11.0f / 8)),
	sinf(PI_F * (12.0f / 8)),
	sinf(PI_F * (13.0f / 8)),
	sinf(PI_F * (14.0f / 8)),
	sinf(PI_F * (15.0f / 8))
};
const float MyRenderer::CosCache16[16] =
{
	cosf(PI_F * (0.0f / 8)),
	cosf(PI_F * (1.0f / 8)),
	cosf(PI_F * (2.0f / 8)),
	cosf(PI_F * (3.0f / 8)),
	cosf(PI_F * (4.0f / 8)),
	cosf(PI_F * (5.0f / 8)),
	cosf(PI_F * (6.0f / 8)),
	cosf(PI_F * (7.0f / 8)),
	cosf(PI_F * (8.0f / 8)),
	cosf(PI_F * (9.0f / 8)),
	cosf(PI_F * (10.0f / 8)),
	cosf(PI_F * (11.0f / 8)),
	cosf(PI_F * (12.0f / 8)),
	cosf(PI_F * (13.0f / 8)),
	cosf(PI_F * (14.0f / 8)),
	cosf(PI_F * (15.0f / 8))
};

MyRenderer::MyRenderer()
	: m_device{}
	, m_primitiveBatch{}
	, m_basicEffect{}
	, m_inputLayout{}
	, m_states{}
	, m_nowMode{ DrawMode::None }
	, m_initialized{ false }
	, m_isDrawStarted{ false }
	, m_isChangeWVP{ false }
	, m_nowContext{ nullptr }
	, m_alphaValue{ 1 }
{
}

void MyRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, DirectX::CommonStates* state)
{
	// 既に初期化済みならスキップ
	if (m_instance.m_initialized) return;
	m_instance.m_initialized = true;

	// スプライトバッチの初期化
	m_instance.m_spriteBatch = std::make_unique<SpriteBatch>(context);

	m_instance.m_device = device;

	m_instance.m_states = state;

	// プリミティブバッチの初期化
	m_instance.m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	// ベーシックエフェクトの初期化
	m_instance.m_basicEffect = std::make_unique<BasicEffect>(m_instance.m_device);

	m_instance.m_basicEffect->SetVertexColorEnabled(true);
	m_instance.m_basicEffect->SetLightingEnabled(false);
	m_instance.m_basicEffect->SetTextureEnabled(false);

	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(
			m_instance.m_device,
			m_instance.m_basicEffect.get(),
			m_instance.m_inputLayout.ReleaseAndGetAddressOf()));

	// WVP行列の初期化
	m_instance.m_basicEffect->SetWorld(SimpleMath::Matrix::Identity);
	m_instance.m_basicEffect->SetView(SimpleMath::Matrix::Identity);
	m_instance.m_basicEffect->SetProjection(SimpleMath::Matrix::Identity);
}

void MyRenderer::StartDraw(
	ID3D11DeviceContext* context	// コンテキスト
)
{
	// 既に開始済みならスキップ
	if (m_instance.m_isDrawStarted) return;

	m_instance.m_nowContext = context;

	// PrimitivBatch　描画モードの設定
	m_instance.m_nowContext->OMSetBlendState(m_instance.m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
	m_instance.m_nowContext->OMSetDepthStencilState(m_instance.m_states->DepthDefault(), 0);
	m_instance.m_nowContext->RSSetState(m_instance.m_states->CullNone());

	m_instance.m_nowContext->IASetInputLayout(m_instance.m_inputLayout.Get());

	// 描画フラグをオンに
	m_instance.m_isDrawStarted = true;

	// α値の初期化
	m_instance.SetAlpha(1.0f);
}

void MyRenderer::EndDraw()
{
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::None);

	m_instance.m_nowContext = nullptr;

	// 描画開始フラグをオフに
	m_instance.m_isDrawStarted = false;
}

void MyRenderer::CheckChange(DrawMode next)
{
	// モードが切り替わる場合
	if (m_instance.m_nowMode != next)
	{
		// 現在のモードの終了処理
		switch (m_instance.m_nowMode)
		{
		case MyRenderer::DrawMode::Primitiv:
		case MyRenderer::DrawMode::UI:
			m_instance.m_primitiveBatch->End();
			break;
		case MyRenderer::DrawMode::Sprite:
		case MyRenderer::DrawMode::Font:
			m_instance.m_spriteBatch->End();

			if (m_instance.m_nowContext)
			{
				// Blend Reset
				m_instance.m_nowContext->OMSetBlendState(m_instance.m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);

				// 2. 深度テストを有効に戻す（スプライト描画でオフになっていることが多いため）
				m_instance.m_nowContext->OMSetDepthStencilState(m_instance.m_states->DepthDefault(), 0);

				// 3. カリング設定などを戻す
				m_instance.m_nowContext->RSSetState(m_instance.m_states->CullNone());

				// 4. サンプラーをリセット
				ID3D11SamplerState* samplers[] = { nullptr };
				m_instance.m_nowContext->PSSetSamplers(0, 1, samplers);
			}
			break;
		case MyRenderer::DrawMode::Model:
			if (m_instance.m_nowContext)
			{
				// レンダリングステートを標準に戻す
				m_instance.m_nowContext->OMSetBlendState(m_instance.m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);				
				m_instance.m_nowContext->OMSetDepthStencilState(m_instance.m_states->DepthDefault(), 0);
				m_instance.m_nowContext->RSSetState(m_instance.m_states->CullNone());

				// Model がセットしたバッファを強制解除する
				ID3D11Buffer* nullBuffers[] = { nullptr };
				UINT strides[] = { 0 };
				UINT offsets[] = { 0 };
				m_instance.m_nowContext->IASetVertexBuffers(0, 1, nullBuffers, strides, offsets);
				m_instance.m_nowContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
			}
			break;
		default:
			break;
		}

		// 次のモードの開始処理
		switch (next)
		{
		case MyRenderer::DrawMode::UI:
			// 行列を単位行列に強制リセット
			m_instance.m_view = SimpleMath::Matrix::Identity;
			m_instance.m_proj = SimpleMath::Matrix::Identity;
			m_instance.m_basicEffect->SetView(m_instance.m_view);
			m_instance.m_basicEffect->SetProjection(m_instance.m_proj);

			// エフェクトを即時適用
			if(m_instance.m_nowContext) m_instance.m_basicEffect->Apply(m_instance.m_nowContext);
			m_instance.m_isChangeWVP = false; // 適用済みなのでフラグを下ろす

			// 共通設定のためbreakなし

		case MyRenderer::DrawMode::Primitiv:

			// InputLayout をプリミティブ用に再設定
			m_instance.m_nowContext->IASetInputLayout(m_instance.m_inputLayout.Get());

			// エフェクトを適用してシェーダーを BasicEffect に戻す
			m_instance.m_basicEffect->Apply(m_instance.m_nowContext);

			m_instance.m_primitiveBatch->Begin();
			break;
		case MyRenderer::DrawMode::Sprite:
		case MyRenderer::DrawMode::Font:
			m_instance.m_spriteBatch->Begin();
			break;
		case MyRenderer::DrawMode::Model:
			break;
		default:
			break;
		}

		m_instance.m_nowMode = next;
	}

	switch (m_instance.m_nowMode)
	{
	case MyRenderer::DrawMode::Primitiv:
	case MyRenderer::DrawMode::UI:

		// 描画開始済みかつ行列の変更がされていたら
		if (m_instance.m_isDrawStarted && m_instance.m_isChangeWVP)
		{
			m_instance.m_primitiveBatch->End();

			if(m_instance.m_nowContext) m_instance.m_basicEffect->Apply(m_instance.m_nowContext);

			m_instance.m_primitiveBatch->Begin();
		}

		break;
	default:
		break;
	}

	// フラグの初期化
	m_instance.m_isChangeWVP = false;
}

DirectX::SimpleMath::Vector2 MyRenderer::GetTextureSize(ID3D11ShaderResourceView* tex)
{
	if (!tex) return DirectX::SimpleMath::Vector2::Zero;

	Microsoft::WRL::ComPtr<ID3D11Resource> res;
	tex->GetResource(res.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
	if (SUCCEEDED(res.As(&tex2d)))
	{
		D3D11_TEXTURE2D_DESC desc;
		tex2d->GetDesc(&desc);
		return DirectX::SimpleMath::Vector2(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
	}
	return DirectX::SimpleMath::Vector2::Zero;
}

void MyRenderer::SetWorld(DirectX::SimpleMath::Matrix world)
{
	m_instance.m_basicEffect->SetWorld(world);
	m_instance.m_isChangeWVP = true;
}

void MyRenderer::SetView(DirectX::SimpleMath::Matrix view)
{
	m_instance.m_view = view;
	m_instance.m_basicEffect->SetView(view);
	m_instance.m_isChangeWVP = true;
}

void MyRenderer::SetProjection(DirectX::SimpleMath::Matrix proj)
{
	m_instance.m_proj = proj;
	m_instance.m_basicEffect->SetProjection(proj);
	m_instance.m_isChangeWVP = true;
}

void MyRenderer::ApplySetting()
{
	m_instance.m_basicEffect->Apply(m_instance.m_nowContext);
}

#pragma region PrimitivBatch

void MyRenderer::DrawTriangle(DirectX::VertexPositionColor p1, DirectX::VertexPositionColor p2, DirectX::VertexPositionColor p3)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Primitiv);

	m_instance.m_primitiveBatch->DrawTriangle(p1, p2, p3);
}

void MyRenderer::DrawTriangle(DirectX::SimpleMath::Vector3 p1, DirectX::SimpleMath::Vector3 p2, DirectX::SimpleMath::Vector3 p3, int color)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Primitiv);

	float r = static_cast<float>((color >> 16) & 0b11111111) / 0xFF;
	float g = static_cast<float>((color >> 8)  & 0b11111111) / 0xFF;
	float b = static_cast<float>((color >> 0)  & 0b11111111) / 0xFF;
	float a = m_instance.m_alphaValue;

	VertexPositionColor v1 = VertexPositionColor(p1, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v2 = VertexPositionColor(p2, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v3 = VertexPositionColor(p3, SimpleMath::Vector4{ r, g, b, a });

	DrawTriangle(v1, v2, v3);
}

void MyRenderer::DrawTriangle2D(DirectX::SimpleMath::Vector2 p1, DirectX::SimpleMath::Vector2 p2, DirectX::SimpleMath::Vector2 p3, int color)
{
	CheckChange(DrawMode::Primitiv);

	SimpleMath::Vector3 s1 = { p1.x, p1.y, 0 };
	SimpleMath::Vector3 s2 = { p2.x, p2.y, 0 };
	SimpleMath::Vector3 s3 = { p3.x, p3.y, 0 };

	DrawTriangle(s1, s2, s3, color);
}

void MyRenderer::DrawRect(DirectX::VertexPositionColor v1, DirectX::VertexPositionColor v2, DirectX::VertexPositionColor v3, DirectX::VertexPositionColor v4)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Primitiv);

	m_instance.m_primitiveBatch->DrawQuad(v1, v2, v3, v4);
}

void MyRenderer::DrawRect(DirectX::SimpleMath::Vector3 p1, DirectX::SimpleMath::Vector3 p2, DirectX::SimpleMath::Vector3 p3, DirectX::SimpleMath::Vector3 p4, int color)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Primitiv);

	float r = static_cast<float>((color >> 16) & 0b11111111) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0b11111111) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0b11111111) / 255.0f;
	float a = m_instance.m_alphaValue;

	VertexPositionColor v1 = VertexPositionColor(p1, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v2 = VertexPositionColor(p2, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v3 = VertexPositionColor(p3, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v4 = VertexPositionColor(p4, SimpleMath::Vector4{ r, g, b, a });

	DrawRect(v1, v2, v3, v4);
}

void MyRenderer::DrawLine(DirectX::VertexPositionColor v1, DirectX::VertexPositionColor v2)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Primitiv);

	m_instance.m_primitiveBatch->DrawLine(v1, v2);
}

void MyRenderer::DrawLine(DirectX::SimpleMath::Vector3 start, DirectX::SimpleMath::Vector3 end, int color)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Primitiv);

	float r = static_cast<float>((color >> 16) & 0b11111111) / 0xFF;
	float g = static_cast<float>((color >> 8) & 0b11111111) / 0xFF;
	float b = static_cast<float>((color >> 0) & 0b11111111) / 0xFF;
	float a = m_instance.m_alphaValue;

	VertexPositionColor v1 = VertexPositionColor(start, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v2 = VertexPositionColor(end, SimpleMath::Vector4{ r, g, b, a });

	m_instance.m_primitiveBatch->DrawLine(v1, v2);
}

void MyRenderer::DrawCircle(DirectX::SimpleMath::Vector3 centerPos, DirectX::SimpleMath::Vector3 normal, float radius, int division, int color, bool fillFrag)
{
	if (division < 3) return;
	CheckChange(DrawMode::Primitiv);

	// --- 色の展開 ---
	float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0xFF) / 255.0f;
	SimpleMath::Vector4 col(r, g, b, m_instance.m_alphaValue);

	// --- 基底ベクトルの計算 ---
	normal.Normalize();
	SimpleMath::Vector3 up = (std::abs(normal.y) > 0.9f) ? SimpleMath::Vector3::UnitX : SimpleMath::Vector3::UnitY;
	SimpleMath::Vector3 vU = normal.Cross(up);
	vU.Normalize();
	SimpleMath::Vector3 vV = normal.Cross(vU);
	vV.Normalize();

	// --- sin/cos のテーブル化 ---
	float step = XM_2PI / static_cast<float>(division);

	DX11::VertexPositionColor* vertexes = new DX11::VertexPositionColor[division * 2];

	DirectX::SimpleMath::Vector3 prevPoint = centerPos + vU * radius;
	// 分割数分ループ
	for (int i = 1; i <= division; ++i)
	{
		float theta = step * i;

		SimpleMath::Vector3 currentPoint;
		currentPoint = centerPos + (vU * cosf(theta) + vV * sinf(theta)) * radius;

		VertexPositionColor v1 = VertexPositionColor(prevPoint, col);
		VertexPositionColor v2 = VertexPositionColor(currentPoint, col);

		vertexes[i * 2 - 2] = v1;
		vertexes[i * 2 - 1] = v2;

		prevPoint = currentPoint;
	}

	m_instance.m_primitiveBatch->Draw(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, vertexes, division * 2);

	delete[] vertexes;
}

void MyRenderer::DrawArc(const DirectX::SimpleMath::Vector3& center, DirectX::SimpleMath::Vector3 vStart, DirectX::SimpleMath::Vector3 vEnd, float radius, int color, int segments)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Primitiv);

	if (radius <= 0.0f || segments <= 0) return;

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
	SimpleMath::Vector4 col(r, g, b, m_instance.m_alphaValue);

	// 描画
	float step = endRadian / static_cast<float>(segments);

	DX11::VertexPositionColor* vertexes = new DX11::VertexPositionColor[segments * 2];

	DirectX::SimpleMath::Vector3 prevPoint = center + vU * radius;
	for (int i = 1; i <= segments; ++i)
	{
		float theta = step * i;

		// 円周上の座標を算出
		DirectX::SimpleMath::Vector3 currentPoint = center + (vU * cosf(theta) + vV * sinf(theta)) * radius;

		VertexPositionColor v1 = VertexPositionColor(prevPoint, col);
		VertexPositionColor v2 = VertexPositionColor(currentPoint, col);

		vertexes[i * 2 - 2] = v1;
		vertexes[i * 2 - 1] = v2;

		prevPoint = currentPoint;
	}

	m_instance.m_primitiveBatch->Draw(D3D10_PRIMITIVE_TOPOLOGY_LINELIST, vertexes, segments * 2);

	delete[] vertexes;
}

#pragma endregion

#pragma region UI

void MyRenderer::DrawBox(DirectX::SimpleMath::Vector2 posA, DirectX::SimpleMath::Vector2 posB, int color)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::UI);

	SimpleMath::Vector2 sA = Screen::GetScreenPos(posA);
	SimpleMath::Vector2 sB = Screen::GetScreenPos(posB);

	SimpleMath::Vector3 drawPosA = { sA.x, sA.y, 0 };
	SimpleMath::Vector3 drawPosB = { sA.x, sB.y, 0 };
	SimpleMath::Vector3 drawPosC = { sB.x, sB.y, 0 };
	SimpleMath::Vector3 drawPosD = { sB.x, sA.y, 0 };

	float r = static_cast<float>((color >> 16) & 0b11111111) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0b11111111) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0b11111111) / 255.0f;
	float a = m_instance.m_alphaValue;

	VertexPositionColor v1 = VertexPositionColor(drawPosA, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v2 = VertexPositionColor(drawPosB, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v3 = VertexPositionColor(drawPosC, SimpleMath::Vector4{ r, g, b, a });
	VertexPositionColor v4 = VertexPositionColor(drawPosD, SimpleMath::Vector4{ r, g, b, a });

	m_instance.m_primitiveBatch->DrawQuad(v1, v2, v3, v4);
}

void MyRenderer::Draw3DBox(DirectX::SimpleMath::Vector3 posA, DirectX::SimpleMath::Vector3 posB, int color, bool fillFrag)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Primitiv);

	SimpleMath::Vector3 pos1 = { posA.x, posA.y, posA.z };
	SimpleMath::Vector3 pos2 = { posA.x, posA.y, posB.z };
	SimpleMath::Vector3 pos3 = { posB.x, posA.y, posB.z };
	SimpleMath::Vector3 pos4 = { posB.x, posA.y, posA.z };
	SimpleMath::Vector3 pos5 = { posA.x, posB.y, posA.z };
	SimpleMath::Vector3 pos6 = { posA.x, posB.y, posB.z };
	SimpleMath::Vector3 pos7 = { posB.x, posB.y, posB.z };
	SimpleMath::Vector3 pos8 = { posB.x, posB.y, posA.z };

	// 塗りつぶしがオンなら
	if (fillFrag)
	{
		// 6面を描画
		DrawRect(pos1, pos2, pos3, pos4, color);	// Up
		DrawRect(pos5, pos6, pos7, pos8, color);	// Down
		DrawRect(pos3, pos4, pos8, pos7, color);	// Front
		DrawRect(pos2, pos1, pos6, pos5, color);	// Back
		DrawRect(pos1, pos4, pos8, pos5, color);	// Left
		DrawRect(pos3, pos2, pos6, pos7, color);	// Right
	}
	else
	{
		// 12辺を描画
		// --- 上面 (Up) ---
		DrawLine(pos1, pos2, color);
		DrawLine(pos2, pos3, color);
		DrawLine(pos3, pos4, color);
		DrawLine(pos4, pos1, color);

		// --- 下面 (Down) ---
		DrawLine(pos5, pos6, color);
		DrawLine(pos6, pos7, color);
		DrawLine(pos7, pos8, color);
		DrawLine(pos8, pos5, color);

		// --- 垂直な柱 (Pillars) ---
		DrawLine(pos1, pos5, color);
		DrawLine(pos2, pos6, color);
		DrawLine(pos3, pos7, color);
		DrawLine(pos4, pos8, color);
	}
}

#pragma endregion

#pragma region Sprite

void MyRenderer::DrawGraph(DirectX::SimpleMath::Vector2 pos, ID3D11ShaderResourceView* texture)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;
	
	CheckChange(DrawMode::Sprite);

	XMVECTOR color = XMVectorSet(1, 1, 1, m_instance.m_alphaValue);
	if(texture) m_instance.m_spriteBatch->Draw(texture, pos, color);
}

void MyRenderer::DrawTurnGraph(DirectX::SimpleMath::Vector2 pos, ID3D11ShaderResourceView* texture)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Sprite);

	SpriteEffects effect = SpriteEffects_FlipHorizontally;

	// 反転描画
	XMVECTOR color = XMVectorSet(1, 1, 1, m_instance.m_alphaValue);

	m_instance.m_spriteBatch->Draw(
		texture, (XMFLOAT2)pos, nullptr, 
		color, 0, { 0, 0 }, 1, effect, 0);
}

void MyRenderer::DrawExtendGraph(DirectX::SimpleMath::Vector2 min, DirectX::SimpleMath::Vector2 max, ID3D11ShaderResourceView* texture)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Sprite);

	RECT rect = { 
		static_cast<LONG>(min.x), 
		static_cast<LONG>(min.y), 
		static_cast<LONG>(max.x), 
		static_cast<LONG>(max.y) };

	XMVECTOR color = XMVectorSet(1, 1, 1, m_instance.m_alphaValue);

	m_instance.m_spriteBatch->Draw(texture, rect, color);
}

#pragma endregion

#pragma region SpriteFont

void MyRenderer::DrawString(SpriteFont* font, const wchar_t* text, DirectX::SimpleMath::Vector2 pos, int color)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	if (!font) return;

	CheckChange(DrawMode::Font);

	// int から Colorへ変換
	float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0xFF) / 255.0f;
	float a = m_instance.m_alphaValue;

	DirectX::SimpleMath::Color dxColor(r, g, b, a);

	font->DrawString(
		m_instance.m_spriteBatch.get(),
		text,
		pos,
		dxColor,
		0.0f,
		{ 0, 0 },
		1.0f
	);
}

void MyRenderer::DrawStringOnCenter(SpriteFont* font, const wchar_t* text, DirectX::SimpleMath::Vector2 pos, int color)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	CheckChange(DrawMode::Font);

	// 位置の計算
	DirectX::SimpleMath::Vector2 size = font->MeasureString(text);

	DirectX::SimpleMath::Vector2 drawPos = { pos.x - size.x / 2, pos.y - size.y / 2 };

	DrawString(font, text, drawPos, color);
}

void MyRenderer::DrawExtendString(DirectX::SpriteFont* font, const wchar_t* text, DirectX::SimpleMath::Vector2 pos, float ratio, int color)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	if (!font) return;

	CheckChange(DrawMode::Font);

	// int から Colorへ変換
	float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0xFF) / 255.0f;
	float a = m_instance.m_alphaValue;

	DirectX::SimpleMath::Color dxColor(r, g, b, a);

	font->DrawString(
		m_instance.m_spriteBatch.get(),
		text,
		pos,
		dxColor,
		0.0f,
		{ 0, 0 },
		ratio
	);
}

void MyRenderer::DrawExtendStringOnCenter(DirectX::SpriteFont* font, const wchar_t* text, DirectX::SimpleMath::Vector2 pos, float ratio, int color)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	if (!font) return;

	CheckChange(DrawMode::Font);

	// int から Colorへ変換
	float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
	float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
	float b = static_cast<float>((color >> 0) & 0xFF) / 255.0f;
	float a = m_instance.m_alphaValue;

	DirectX::SimpleMath::Color dxColor(r, g, b, a);

	// 位置の計算
	DirectX::SimpleMath::Vector2 size = font->MeasureString(text);

	DirectX::SimpleMath::Vector2 drawPos = { pos.x - size.x * ratio / 2, pos.y - size.y * ratio / 2 };

	font->DrawString(
		m_instance.m_spriteBatch.get(),
		text,
		drawPos,
		dxColor,
		0.0f,
		{ 0, 0 },
		ratio
	);
}

void MyRenderer::DrawModel(DirectX::Model* model, DirectX::SimpleMath::Matrix world)
{
	// 初期化していなければスキップ
	if (!m_instance.m_initialized) return;

	// 描画開始していなければスキップ
	if (!m_instance.m_isDrawStarted) return;

	if (!model) return;

	// モードをModelに切り替え
	CheckChange(DrawMode::Model);

	model->Draw(
		m_instance.m_nowContext,
		*m_instance.m_states,
		world,
		m_instance.m_view, m_instance.m_proj
	);
}

void MyRenderer::DrawModel(DirectX::Model* model, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot, float scale)
{
	SimpleMath::Matrix tMatrix = SimpleMath::Matrix::CreateTranslation(pos);
	SimpleMath::Matrix rMatrix = SimpleMath::Matrix::CreateFromYawPitchRoll(rot);
	SimpleMath::Matrix sMatrix = SimpleMath::Matrix::CreateScale(scale);

	SimpleMath::Matrix world = sMatrix * rMatrix * tMatrix;

	DrawModel(model, world);
}

#pragma endregion

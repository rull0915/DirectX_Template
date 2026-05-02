//====================================================//
// ファイル名   : RenderProxy.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/29
//
// 概要 : 描画の関数を呼び出す仲介関数
//
// 更新履歴 :
// 2026/04/29 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "EachRenderer/Primitive/PrimitiveRenderer.h"
#include "EachRenderer/Sprite/SpriteRenderer.h"
#include "EachRenderer/Model/ModelRenderer.h"

#include <utility>

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// 列挙型宣言
//====================================================//
enum class RendererType
{
    None,
    Primitive,
    Model,
    Sprite,
};

//====================================================//
// クラス宣言
//====================================================//
class RenderProxy
{
private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------
    PrimitiveRenderer* m_primitive;
    ModelRenderer* m_model;
    SpriteRenderer* m_sprite;

    void* m_renderer;
    void(*m_switchRenderer)(void*, RendererType);

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    RenderProxy(void* renderer, void(*switchRenderer)(void*, RendererType));
    ~RenderProxy() = default;

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

    void SetPrimitive(PrimitiveRenderer* primitive) { m_primitive = primitive; }
    void SetModel(ModelRenderer* model) { m_model = model; }
    void SetSprite(SpriteRenderer* sprite) { m_sprite = sprite; }

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // ----------- PrimitiveRenderer ------------- //

    // Triangle
    void Triangle(DirectX::VertexPositionColor v1, DirectX::VertexPositionColor v2, DirectX::VertexPositionColor v3, bool fillFrag = true){
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawTriangle(v1, v2, v3, fillFrag);}
    void Triangle(DirectX::SimpleMath::Vector3 p1, DirectX::SimpleMath::Vector3 p2, DirectX::SimpleMath::Vector3 p3, int color, bool fillFrag = true){
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawTriangle(p1, p2, p3, color, fillFrag);}

    // Rect
    void Rect(DirectX::VertexPositionColor v1, DirectX::VertexPositionColor v2, DirectX::VertexPositionColor v3, DirectX::VertexPositionColor v4, bool fillFrag = true) {
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawRect(v1, v2, v3, v4, fillFrag);}
    void Rect(DirectX::SimpleMath::Vector3 p1, DirectX::SimpleMath::Vector3 p2, DirectX::SimpleMath::Vector3 p3, DirectX::SimpleMath::Vector3 p4, int color, bool fillFrag = true) {
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawRect(p1, p2, p3, p4, color, fillFrag);}

    // Line
    void Line(DirectX::VertexPositionColor v1, DirectX::VertexPositionColor v2) {
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawLine(v1, v2); }
    void Line(DirectX::SimpleMath::Vector3 start, DirectX::SimpleMath::Vector3 end, int color){
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawLine(start, end, color);}

    // Circle
    void Circle(DirectX::SimpleMath::Vector3 centerPos, DirectX::SimpleMath::Vector3 normal, float radius, int division, int color, bool fillFrag = true){
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawCircle(centerPos, normal, radius, division, color, fillFrag);}

    // Arc
    void Arc(const DirectX::SimpleMath::Vector3& center, DirectX::SimpleMath::Vector3 vStart, DirectX::SimpleMath::Vector3 vEnd, int division, float radius, int color, bool fillFlag = true) {
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawArc(center, vStart, vEnd, division, radius, color, fillFlag);}

    // Index
    void PrimitiveIndex(D3D11_PRIMITIVE_TOPOLOGY topology, uint16_t* indexes, uint16_t indexCount, DirectX::VertexPositionColor* vertexes, uint16_t vertexCount) {
        m_switchRenderer(m_renderer, RendererType::Primitive);
        m_primitive->DrawIndex(topology, indexes, indexCount, vertexes, vertexCount);
    }

    // ------------- ModelRenderer --------------- //

    void Model(DirectX::Model* model, DirectX::SimpleMath::Matrix world) {
        m_switchRenderer(m_renderer, RendererType::Model);
        m_model->DrawModel(model, world);}
};
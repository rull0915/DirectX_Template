//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "GameLib/Resources/ResourceManager.h"

#include "GameLib/GameObject/3D/ObjectManager.h"
#include "GameLib/GameObject/2D/ObjectManager2D.h"

#include "GameLib/Input/KeyInput.h"
#include "GameLib/Input/MouseInput.h"

#include "Game/Screen.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
    : m_states{}
    , m_stageCount{ 0 }
, m_exitTrans{ nullptr }
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

/// <summary>
/// デストラクタ
/// </summary>
Game::~Game()
{
    ObjectManager::Instance().Finalize();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    MouseInput::SetMode(DirectX::Mouse::MODE_ABSOLUTE);

    // ====== シーンの登録 ====== //
    m_sceneManager.RegisterScene("Test", std::make_unique<TestScene>(this));

    // 開始時のシーンを設定
    m_sceneManager.SetStartScene("Test");

    // ====== リソースの追加 ====== //

    // テクスチャの追加

    // フォントの追加

    // モデルの追加

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.

    // 終了チェック
    if (m_exitTrans)
    {
        if (m_exitTrans->Update(elapsedTime)) ExitGame();
    }

    // 入力情報の更新
    KeyInput::KeyUpdate();
    MouseInput::MouseUpdate();

    // デバッグ用の一時停止処理
    static bool stop = false;
    if (KeyInput::GetKeyDown(DirectX::Keyboard::Keys::F1)) stop = !stop;

    if (stop)
    {
        if (KeyInput::GetKeyDown(DirectX::Keyboard::Keys::F3))
        {
            // 各シーンの更新
            m_sceneManager.Update(elapsedTime);
            ObjectManager::Instance().Update(elapsedTime);
            ObjectManager2D::Instance().Update(elapsedTime);
        }
        return;
    }

    // 各シーンの更新
    m_sceneManager.Update(elapsedTime);
    ObjectManager::Instance().Update(elapsedTime);
    ObjectManager2D::Instance().Update(elapsedTime);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

    // 描画の開始 ----------------------------------------
    MyRenderer::StartDraw(context);

    // 現在のシーンの描画
    m_sceneManager.Render();

    ObjectManager::Instance().Render();
    ObjectManager2D::Instance().Render();

    // 描画の終了 ----------------------------------------
    m_sceneManager.TransitionRender();
    if (m_exitTrans) m_exitTrans->Render();
    m_deviceResources->PIXEndEvent();
    MyRenderer::EndDraw();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    DirectX::XMVECTORF32 color = { 0.6f, 0.6f, 0.7f, 1.0f };
    context->ClearRenderTargetView(renderTarget, color);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    const auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    const auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = Screen::WIDTH;
    height = Screen::HEIGHT;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).

    m_states = std::make_unique<CommonStates>(device);

    // 描画関連クラスの初期化
    MyRenderer::Initialize(device, context, m_states.get());

    // 画面サイズを取得する
    int w, h;
    GetDefaultSize(w, h);

    // リソースマネージャの初期化
    ResourceManager::Instance().Initialize(device);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

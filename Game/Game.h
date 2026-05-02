//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include <memory>

#include "GameLib/MyRenderer.h"

#include "GameLib/Scene/SceneManager.h"
#include "GameLib/Scene/Scenes.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
    // ========= テンプレートの処理 ========= //
public:

    Game() noexcept(false);
    ~Game();

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize(int& width, int& height) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // FPSカウンタ
    uint32_t m_frameCount;
    float m_timeAccumulator;
    float m_fps;

    // ================ 自分の処理 ================== //
    
    // タイトルの文字列
public:
    static constexpr const wchar_t TITLE_STRING[]{ L"Template" };

private:
    // シーンマネージャー
    SceneManager m_sceneManager;

    // コモンステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    int m_stageCount;

    std::unique_ptr<SceneTransition> m_exitTrans;

public:
    // シーンマネージャーを返す関数
    SceneManager* GetSceneManager() { return &m_sceneManager; }

    // 描画担当
    Renderer m_renderer;

    void RequestExit()
    {
        m_exitTrans = std::make_unique<FadeTransition>(1.0f, Transition::Mode::In, 0x000000);
    };

private:
    void TitleNameUpdate(float elapsedTime);
};

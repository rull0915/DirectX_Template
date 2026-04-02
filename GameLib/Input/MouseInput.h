#pragma once

#define MOUSE_LEFT 0
#define MOUSE_MIDDLE 1
#define MOUSE_RIGHT 2

// キー入力管理クラス
class MouseInput
{
private:
	static DirectX::Mouse::State m_nowMouseState;
	static DirectX::Mouse::State m_oldMouseState;

public:
	// ----- 関数宣言 ----- //
	MouseInput();

	// キー情報の更新
	static void MouseUpdate();

	// --- マウス情報取得関数 --- //

	// 押された際
	static bool GetMouseDown(int MouseCode);

	// 押されているか
	static bool GetMouse(int MouseCode);

	// 離された際
	static bool GetMouseUp(int MouseCode);

	// 位置
	static DirectX::SimpleMath::Vector2 GetMousePoint();
	static DirectX::SimpleMath::Vector2 GetOldMousePoint();

	// 移動量
	static DirectX::SimpleMath::Vector2 GetMouseMoveValue();
	static int GetWheelMoveValue();

	// モードの変更
	static void SetMode(DirectX::Mouse::Mode mode);
};


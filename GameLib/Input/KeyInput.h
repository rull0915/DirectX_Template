#pragma once

class KeyInput
{
private:
	static DirectX::Keyboard::State m_nowState;
	static DirectX::Keyboard::State m_oldState;

public:
	// ----- 関数宣言 ----- //

	KeyInput();

	~KeyInput() = default;

	// キー情報の更新
	static void KeyUpdate();

	// --- キー情報取得関数 --- //

	// 押された際
	static bool GetKeyDown(DirectX::Keyboard::Keys key);

	// 押されているか
	static bool GetKey(DirectX::Keyboard::Keys key);

	// 離された際
	static bool GetKeyUp(DirectX::Keyboard::Keys key);
};

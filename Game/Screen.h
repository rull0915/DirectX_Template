#pragma once

class Screen 
{
public:
	static constexpr int WIDTH = 1280;
	static constexpr int HEIGHT = 720;

	// -1 ~ +1 の座標系からピクセル座標に変換する関数
	static DirectX::SimpleMath::Vector2 GetPixelPos(DirectX::SimpleMath::Vector2 pos) 
	{
		float xRatio = (pos.x + 1) / 2;
		float yRatio = (pos.y + 1) / 2;

		return { WIDTH * xRatio, HEIGHT * yRatio };
	}
	// ピクセル座標から -1 ~ +1 の座標系に変換する関数
	static DirectX::SimpleMath::Vector2 GetScreenPos(DirectX::SimpleMath::Vector2 pos)
	{
		float xRatio = pos.x / WIDTH;
		float yRatio = pos.y / HEIGHT;

		return { xRatio * 2 - 1, -(yRatio * 2 - 1) };
	}
};
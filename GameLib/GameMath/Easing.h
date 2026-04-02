#pragma once
#include <cmath>

#define PI_F 3.14159265f

/* イージング構造体
* 参考: https://easings.net/ja
* 使用方法 : 
*   0.0 ~ 1.0に正規化された値を引数に渡してください
*   変換後の値が戻り値となります。
* 
* Date: 2026/02/24/15:32
* Hoshino
*/

struct Easing
{
    // ====== 定数宣言 ====== //
private:
    static constexpr float c1 = 1.70158f;
    static constexpr float c2 = c1 * 1.525f;
    static constexpr float c3 = c1 + 1.0f;
    static constexpr float c4 = (2.0f * PI_F) / 3.0f;
    static constexpr float c5 = (2.0f * PI_F) / 4.5f;

public:

    // ==== Linear ==== //
    static float Linear(float t) { return t; }

    // ===== Sine ===== //
    static float EaseInSine(float t)
    {
        return 1.0f - cosf((t * PI_F) / 2.0f);
    }

    static float EaseOutSine(float t)
    {
        return sinf((t * PI_F) / 2.0f);
    }

    static float EaseInOutSine(float t)
    {
        return -(cosf(PI_F * t) - 1.0f) / 2.0f;
    }

    // ===== Quad ===== //
    static float EaseInQuad(float t)
    {
        return t * t;
    }

    static float EaseOutQuad(float t)
    {
        return 1 - (1 - t) * (1 - t);
    }

    static float EaseInOutQuad(float t)
    {
        return t < 0.5f
            ? 2 * t * t
            : 1 - powf(-2 * t + 2, 2) / 2;
    }

    // ===== Cubic ===== //
    static float EaseInCubic(float t)
    {
        return t * t * t;
    }

    static float EaseOutCubic(float t)
    {
        return 1 - powf(1 - t, 3);
    }

    static float EaseInOutCubic(float t)
    {
        return t < 0.5f
            ? 4 * t * t * t
            : 1 - powf(-2 * t + 2, 3) / 2;
    }

    // ===== Quart ===== //
    static float EaseInQuart(float t)
    {
        return t * t * t * t;
    }

    static float EaseOutQuart(float t)
    {
        return 1 - powf(1 - t, 4);
    }

    static float EaseInOutQuart(float t)
    {
        return t < 0.5f
            ? 8 * t * t * t * t
            : 1 - powf(-2 * t + 2, 4) / 2;
    }

    // ===== Quint ===== //
    static float EaseInQuint(float t)
    {
        return t * t * t * t * t;
    }

    static float EaseOutQuint(float t)
    {
        return 1 - powf(1 - t, 5);
    }

    static float EaseInOutQuint(float t)
    {
        return t < 0.5f
            ? 16 * t * t * t * t * t
            : 1 - powf(-2 * t + 2, 5) / 2;
    }

    // ===== Expo ===== //
    static float EaseInExpo(float t)
    {
        return t == 0 ? 0 : powf(2, 10 * t - 10);
    }

    static float EaseOutExpo(float t)
    {
        return t == 1 ? 1 : 1 - powf(2, -10 * t);
    }

    static float EaseInOutExpo(float t)
    {
        if (t == 0) return 0;
        if (t == 1) return 1;
        return t < 0.5f
            ? powf(2, 20 * t - 10) / 2
            : (2 - powf(2, -20 * t + 10)) / 2;
    }

    // ===== Circ ===== //
    static float EaseInCirc(float t)
    {
        return 1 - sqrtf(1 - t * t);
    }

    static float EaseOutCirc(float t)
    {
        return sqrtf(1 - powf(t - 1, 2));
    }

    static float EaseInOutCirc(float t)
    {
        return t < 0.5f
            ? (1 - sqrtf(1 - powf(2 * t, 2))) / 2
            : (sqrtf(1 - powf(-2 * t + 2, 2)) + 1) / 2;
    }

    // ===== Back ===== //
    static float EaseInBack(float t)
    {
        return c3 * t * t * t - c1 * t * t;
    }

    static float EaseOutBack(float t)
    {
        return 1 + c3 * powf(t - 1, 3) + c1 * powf(t - 1, 2);
    }

    static float EaseInOutBack(float t)
    {
        return t < 0.5f
            ? (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
            : (powf(2 * t - 2, 2) * ((c2 + 1) * (2 * t - 2) + c2) + 2) / 2;
    }

    // ===== Elastic ===== //
    static float EaseInElastic(float t)
    {
        if (t == 0) return 0;
        if (t == 1) return 1;
        return -powf(2, 10 * t - 10) * sinf((t * 10 - 10.75f) * c4);
    }

    static float EaseOutElastic(float t)
    {
        if (t == 0) return 0;
        if (t == 1) return 1;
        return powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1;
    }

    static float EaseInOutElastic(float t)
    {
        if (t == 0) return 0;
        if (t == 1) return 1;
        return t < 0.5f
            ? -(powf(2, 20 * t - 10) * sinf((20 * t - 11.125f) * c5)) / 2
            : (powf(2, -20 * t + 10) * sinf((20 * t - 11.125f) * c5)) / 2 + 1;
    }

    // ===== Bounce ===== //
    static float EaseOutBounce(float t)
    {
        const float n1 = 7.5625f;
        const float d1 = 2.75f;

        if (t < 1 / d1)
            return n1 * t * t;
        else if (t < 2 / d1)
        {
            t -= 1.5f / d1;
            return n1 * t * t + 0.75f;
        }
        else if (t < 2.5f / d1)
        {
            t -= 2.25f / d1;
            return n1 * t * t + 0.9375f;
        }
        else
        {
            t -= 2.625f / d1;
            return n1 * t * t + 0.984375f;
        }
    }

    static float EaseInBounce(float t)
    {
        return 1 - EaseOutBounce(1 - t);
    }

    static float EaseInOutBounce(float t)
    {
        return t < 0.5f
            ? (1 - EaseOutBounce(1 - 2 * t)) / 2
            : (1 + EaseOutBounce(2 * t - 1)) / 2;
    }
};
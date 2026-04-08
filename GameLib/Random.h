#pragma once

namespace Random
{
    void Init();
    int Get(int min, int max);
    long long GetLong(long long min, long long max);
    float GetFloat(float min, float max);
    bool GetBool(float pct);
    double GetDouble(double min, double max);
}

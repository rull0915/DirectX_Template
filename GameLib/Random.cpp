#include <random>
#include "Random.h"

namespace Random
{
    // staticにすることで、プログラム全体でたった1つのエンジンが共有されます。
    static std::mt19937_64 mt;

    // TODO: 以下の処理を実装してください
    void Init()
    {
       // 1. std::random_deviceを作成（変数名 rd）
        std::random_device rd;

        // 2. 上記のrdで4つの値を使ってstd::seed_seqを作成（変数名 ss）
        std::seed_seq ss{ rd(), rd(), rd(), rd() };
        
        // 3. 上記のss連続値を使って「mt」エンジンの再シードを行う（mt.seed()関数）
        mt.seed(ss);
    }

    // ヘルパー関数
    int Get(int min, int max)
    {
        std::uniform_int_distribution<int> die{ min, max };

        // ここでも mt 関数経由でエンジンを渡します
        return die(mt);
    }

    long long GetLong(long long min, long long max)
    {
        std::uniform_int_distribution<long long> die{ min, max };

        return die(mt);
    }
    bool GetBool(float pct)
    {
        std::bernoulli_distribution die{ pct };

        return die(mt);
    }
    double GetDouble(double min, double max)
    {
        std::uniform_real_distribution<double> die{ min, max };

        return die(mt);
    }
}

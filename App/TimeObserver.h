
#include <chrono>

/// <summary>
/// 処理時間計測クラス
/// </summary>
class TimeObserver
{
    std::chrono::high_resolution_clock::time_point  mStart, mEnd;
public:
    /// <summary>
    /// コンストラクタ
    /// 宣言と同時に計測が始まる
    /// </summary>
    TimeObserver()
    {
        mStart = std::chrono::high_resolution_clock::now();
    }

    /// <summary>
    /// 計測終了
    /// 計測を終了してコンストラクタが呼ばれてからの時間を返す
    /// </summary>
    /// <returns>Nms(ミリ秒)</returns>
    float GetTimeFromStart()
    {
        mEnd = std::chrono::high_resolution_clock::now();
        return static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>
            (mEnd - mStart).count()) / 1000.0f;
    }
};
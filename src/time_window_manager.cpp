#include "../include/time_window_manager.h"

TimeWindowManager::TimeWindowManager(int windowSize)
    : maxWindowSize(windowSize), currTime(-1), curWindowSize(0) {}

bool TimeWindowManager::shouldRemoveOld(int newTime)
{
    // 窗口未满且时间变化需要扩大窗口
    if (curWindowSize < maxWindowSize && currTime != newTime)
    {
        currTime = newTime;
        ++curWindowSize;
        return false;
    }

    // 窗口已满且时间变化需要移除过期词
    if (currTime != newTime)
    {
        currTime = newTime;
        return true;
    }

    return false;
}

std::vector<Tword> TimeWindowManager::getAndRemoveOutdatedWords()
{
    std::vector<Tword> outdated;
    int threshold = currTime - maxWindowSize;

    // 移除所有时间戳 <= threshold 的词
    while (!historyQueue.empty() && historyQueue.front().first <= threshold)
    {
        outdated.push_back(historyQueue.front());
        historyQueue.pop();
    }

    return outdated;
}

void TimeWindowManager::addWord(int timestamp, const std::string &word)
{
    historyQueue.push({timestamp, word});
}

#ifndef TIME_WINDOW_MANAGER_H
#define TIME_WINDOW_MANAGER_H

#include <queue>
#include <string>
#include <utility>
#include <vector>

using Tword = std::pair<int, std::string>;

class TimeWindowManager
{
    private:
        std::queue<Tword> historyQueue;  // 存储所有在窗口内的词
        int maxWindowSize;               // 最大窗口大小
        int currTime = -1;               // 当前时间戳
        int curWindowSize = 0;           // 当前窗口大小

    public:
        TimeWindowManager(int windowSize);
        
        // 判断是否需要移除过期词
        bool shouldRemoveOld(int newTime);
        
        // 获取并移除过期的词（返回需要删除的词列表）
        std::vector<Tword> getAndRemoveOutdatedWords();
        
        // 添加新词到窗口
        void addWord(int timestamp, const std::string& word);
        
        // Getter方法
        int getCurrentTime() const { return currTime; }
        int getCurrentWindowSize() const { return curWindowSize; }
        bool isEmpty() const { return historyQueue.empty(); }
};

#endif
#ifndef WORD_RANKER_H
#define WORD_RANKER_H

#include <set>
#include <vector>
#include <utility>
#include <string>
#include <unordered_map>

using Tword = std::pair<int, std::string>;

class WordRanker
{
    private:
        std::unordered_map<std::string, int> freqMap; // 词频映射
        std::set<Tword> rankingSet;                   // 按频次排序的集合 (freq, word)

    public:
        WordRanker() = default;

        // 添加一个词（词频+1）
        void addWord(const std::string &word);

        // 移除一个词（词频-1，如果为0则完全删除）
        void removeWord(const std::string &word);

        // 获取TopK词汇
        std::vector<std::pair<std::string, int>> getTopK(int k) const;

        // 获取排名集合（用于输出格式化）
        const std::set<Tword> &getRankingSet() const { return rankingSet; }
};

#endif
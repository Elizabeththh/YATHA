#include "../include/word_ranker.h"

void WordRanker::addWord(const std::string &word)
{
    if (freqMap.count(word))
    {
        // 词已存在，频次+1
        int oldCount = freqMap[word];
        rankingSet.erase({oldCount, word});
        rankingSet.insert({oldCount + 1, word});
        ++freqMap[word];
    }
    else
    {
        // 新词，频次为1
        freqMap[word] = 1;
        rankingSet.insert({1, word});
    }
}

void WordRanker::removeWord(const std::string &word)
{
    if (!freqMap.count(word))
        return; // 词不存在，直接返回

    int count = freqMap[word];
    rankingSet.erase({count, word});

    if (count == 1)
        // 频次降为0，完全删除
        freqMap.erase(word);
    else
    {
        // 频次-1
        --freqMap[word];
        rankingSet.insert({count - 1, word});
    }
}

std::vector<std::pair<std::string, int>> WordRanker::getTopK(int k) const
{
    std::vector<std::pair<std::string, int>> result;
    result.reserve(k);

    auto it = rankingSet.rbegin();
    for (int i = 0; i < k && it != rankingSet.rend(); ++i, ++it)
        result.push_back({it->second, it->first}); 

    return result;
}

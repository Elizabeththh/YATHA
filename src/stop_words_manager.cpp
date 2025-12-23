#include "../include/stop_words_manager.h"
#include <iostream>

StopWordsManager::StopWordsManager(const std::string& stopWordsDict)
{
    std::ifstream ifs(stopWordsDict);
    if(!ifs.is_open())
    {
        std::cerr << "无法打开停用词词典 " << stopWordsDict;
        exit(1);
    }
    std::string word;
    while(getline(ifs, word))
    {
        if(!word.empty() && word.back() == '\r')
            word.pop_back();
        if(!word.empty())
            stopWords.insert(word);
    }
}

bool StopWordsManager::isStopWord(std::string& word)
{
    if(stopWords.count(word))
        return true;
    return false;
}
#include "../third_party/cppjieba/Jieba.hpp"
#include "../include/ha_engine.hpp"
#include <iostream>

using namespace std;



int main()
{
    std::string inputFile = "../test_sentences.txt";
    std::string outputFile = "output.txt";
    int topK = 3;
    int windowSize = 600;

    std::ofstream out(outputFile, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "[ERROR] cannot open output file: " << outputFile << std::endl;
        return EXIT_FAILURE;
    }

    HaEngine ha(windowSize, topK, inputFile, outputFile);
    ha.jieba.DeleteUserWord("技术创新");
    ha.cutWord();
    // ha.cutWordsTest();
    // ha.testOutput();
}
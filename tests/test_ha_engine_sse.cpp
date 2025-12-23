#define CATCH_CONFIG_MAIN
#include "catch2/catch_test_macros.hpp"
#include "../include/ha_engine_sse.h"
#include "../third_party/json.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

std::string createTestFile(const std::vector<std::string>& lines) {
    static int fileCounter = 0;
    std::string filepath = "data/tests/test_sse_" + std::to_string(fileCounter++) + ".txt";
    std::ofstream file(filepath, std::ios::binary);
    for (const auto& line : lines) {
        file << line << "\n";
    }
    file.close();
    return filepath;
}

json readJsonOutput(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return json::object();
    }
    json result;
    try {
        file >> result;
    } catch (...) {
        return json::object();
    }
    file.close();
    return result;
}

void cleanupFile(const std::string& filepath) {
    if (fs::exists(filepath)) {
        fs::remove(filepath);
    }
}

int getWordCount(const json& output, const std::string& word) {
    if (output.contains(word)) {
        return output[word].get<int>();
    }
    return 0;
}

TEST_CASE("cutWord - 基本步进功能", "[cutWord][Step]") {
    std::unordered_set<std::string> empty_filter;
    std::unordered_set<std::string> empty_chooser;
    
    SECTION("step=10秒 第一次处理") {
        std::vector<std::string> lines = {
            "[0:00:00] 苹果 苹果",
            "[0:00:05] 香蕉 香蕉 香蕉",
            "[0:00:09] 橙子 橙子",
            "[0:00:15] 葡萄 葡萄 葡萄 葡萄"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_1.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        engine.cutWord();
        json output1 = readJsonOutput(outputFile);
        
        REQUIRE(getWordCount(output1, "苹果") == 2);
        REQUIRE(getWordCount(output1, "香蕉") == 3);
        REQUIRE(getWordCount(output1, "橙子") == 2);
        REQUIRE(getWordCount(output1, "葡萄") == 0);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("step=10秒 多次调用增量处理") {
        std::vector<std::string> lines = {
            "[0:00:00] 西瓜 西瓜",
            "[0:00:05] 西瓜 西瓜",
            "[0:00:15] 芒果 芒果 芒果",
            "[0:00:20] 芒果 芒果",
            "[0:00:30] 榴莲 榴莲 榴莲 榴莲"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_2.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        // 第一次：处理 0-10 秒
        engine.cutWord();
        json output1 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output1, "西瓜") == 4);
        REQUIRE(getWordCount(output1, "芒果") == 0);
        REQUIRE(getWordCount(output1, "榴莲") == 0);
        
        // 第二次：处理 15-25 秒，累积所有数据
        engine.cutWord();
        json output2 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output2, "西瓜") == 4);
        REQUIRE(getWordCount(output2, "芒果") == 5);
        REQUIRE(getWordCount(output2, "榴莲") == 0);
        
        // 第三次：处理 30-40 秒
        engine.cutWord();
        json output3 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output3, "西瓜") == 4);
        REQUIRE(getWordCount(output3, "芒果") == 5);
        REQUIRE(getWordCount(output3, "榴莲") == 4);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("step=1秒 精确步进") {
        std::vector<std::string> lines = {
            "[0:00:00] 番茄 番茄",
            "[0:00:01] 黄瓜 黄瓜 黄瓜",
            "[0:00:02] 茄子 茄子 茄子 茄子"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_3.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 1  // step=1秒
        );
        
        engine.cutWord();
        json output1 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output1, "番茄") == 2);
        REQUIRE(getWordCount(output1, "黄瓜") == 0);
        
        engine.cutWord();
        json output2 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output2, "番茄") == 2);
        REQUIRE(getWordCount(output2, "黄瓜") == 3);
        REQUIRE(getWordCount(output2, "茄子") == 0);
        
        engine.cutWord();
        json output3 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output3, "番茄") == 2);
        REQUIRE(getWordCount(output3, "黄瓜") == 3);
        REQUIRE(getWordCount(output3, "茄子") == 4);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("step=5秒 中等步长") {
        std::vector<std::string> lines = {
            "[0:00:00] 荔枝 荔枝",
            "[0:00:03] 荔枝 荔枝",
            "[0:00:04] 龙眼 龙眼",
            "[0:00:06] 椰子 椰子 椰子"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_4.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 5
        );
        
        // 第一次：0-5秒，不包含5秒
        engine.cutWord();
        json output1 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output1, "荔枝") == 4);
        REQUIRE(getWordCount(output1, "龙眼") == 2);
        REQUIRE(getWordCount(output1, "椰子") == 0);
        
        // 第二次：6-11秒
        engine.cutWord();
        json output2 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output2, "椰子") == 3);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
}

TEST_CASE("cutWord - 边界条件", "[cutWord][Boundary]") {
    std::unordered_set<std::string> empty_filter;
    std::unordered_set<std::string> empty_chooser;
    
    SECTION("时间戳正好在边界上") {
        std::vector<std::string> lines = {
            "[0:00:00] 火龙果 火龙果",
            "[0:00:10] 猕猴桃 猕猴桃 猕猴桃",
            "[0:00:11] 木瓜 木瓜"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_5.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        // 0到10秒（不含10秒）
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output, "火龙果") == 2);
        REQUIRE(getWordCount(output, "猕猴桃") == 0);  // 10秒不应被包含
        REQUIRE(getWordCount(output, "木瓜") == 0);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("空输入文件") {
        std::vector<std::string> lines = {};
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_6.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        REQUIRE(output.empty());
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("单行数据") {
        std::vector<std::string> lines = {
            "[0:00:00] 杨梅 杨梅 杨梅 杨梅 杨梅"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_7.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output, "杨梅") == 5);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("所有数据在同一时间戳") {
        std::vector<std::string> lines = {
            "[0:00:00] 草莓 草莓",
            "[0:00:00] 蓝莓 蓝莓 蓝莓",
            "[0:00:00] 樱桃 樱桃 樱桃 樱桃"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_8.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output, "草莓") == 2);
        REQUIRE(getWordCount(output, "蓝莓") == 3);
        REQUIRE(getWordCount(output, "樱桃") == 4);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
}

TEST_CASE("cutWord - 大步长", "[cutWord][LargeStep]") {
    std::unordered_set<std::string> empty_filter;
    std::unordered_set<std::string> empty_chooser;
    
    SECTION("step=9999 一次处理所有数据") {
        std::vector<std::string> lines = {
            "[0:00:00] 山竹 山竹",
            "[0:01:00] 红枣 红枣 红枣",
            "[0:05:00] 柿子 柿子 柿子 柿子",
            "[0:10:00] 枇杷 枇杷 枇杷 枇杷 枇杷"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_9.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 9999
        );
        
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output, "山竹") == 2);
        REQUIRE(getWordCount(output, "红枣") == 3);
        REQUIRE(getWordCount(output, "柿子") == 4);
        REQUIRE(getWordCount(output, "枇杷") == 5);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("step=60秒 一分钟步长") {
        std::vector<std::string> lines = {
            "[0:00:00] 梨 梨",
            "[0:00:30] 梨 梨",
            "[0:00:59] 梨 梨",
            "[0:01:00] 李子 李子"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_10.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 60
        );
        
        engine.cutWord();
        json output1 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output1, "梨") == 6);
        REQUIRE(getWordCount(output1, "李子") == 0);
        
        engine.cutWord();
        json output2 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output2, "李子") == 2);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
}

TEST_CASE("cutWord - 时间解析", "[cutWord][TimeFormat]") {
    std::unordered_set<std::string> empty_filter;
    std::unordered_set<std::string> empty_chooser;
    
    SECTION("跨小时边界") {
        std::vector<std::string> lines = {
            "[0:59:55] 杏 杏",
            "[1:00:00] 栗子 栗子 栗子",
            "[1:00:05] 核桃 核桃"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_11.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 20
        );
        
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output, "杏") == 2);
        REQUIRE(getWordCount(output, "栗子") == 3);
        REQUIRE(getWordCount(output, "核桃") == 2);  // 超出范围
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("从非零小时开始") {
        std::vector<std::string> lines = {
            "[2:30:00] 松子 松子",
            "[2:30:05] 哈密瓜 哈密瓜 哈密瓜"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_12.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output, "松子") == 2);
        REQUIRE(getWordCount(output, "哈密瓜") == 3);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
}

TEST_CASE("cutWord - TopK限制", "[cutWord][TopK]") {
    std::unordered_set<std::string> empty_filter;
    std::unordered_set<std::string> empty_chooser;
    
    SECTION("TopK=3 只输出前3个高频词") {
        std::vector<std::string> lines = {
            "[0:00:00] 菠萝 菠萝 菠萝 菠萝 菠萝 柠檬 柠檬 柠檬 柠檬 石榴 石榴 石榴 柚子 柚子 桃子"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_13.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 3,  // topK=3
            empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        
        REQUIRE(output.size() == 3);
        REQUIRE(getWordCount(output, "菠萝") == 5);
        REQUIRE(getWordCount(output, "柠檬") == 4);
        REQUIRE(getWordCount(output, "石榴") == 3);
        // 柚子 和 桃子 不应出现
        REQUIRE(getWordCount(output, "柚子") == 0);
        REQUIRE(getWordCount(output, "桃子") == 0);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
    
    SECTION("TopK=1 只输出最高频词") {
        std::vector<std::string> lines = {
            "[0:00:00] 甜瓜 甜瓜 甜瓜 枣 枣 山楂"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_14.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 1,  // topK=1
            empty_filter, empty_chooser,
            inputFile, outputFile, 10
        );
        
        engine.cutWord();
        json output = readJsonOutput(outputFile);
        
        REQUIRE(output.size() == 1);
        REQUIRE(getWordCount(output, "甜瓜") == 3);
        REQUIRE(getWordCount(output, "枣") == 0);
        REQUIRE(getWordCount(output, "山楂") == 0);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
}

TEST_CASE("cutWord - 连续空隙数据", "[cutWord][Gap]") {
    std::unordered_set<std::string> empty_filter;
    std::unordered_set<std::string> empty_chooser;
    
    SECTION("数据间有大间隔") {
        std::vector<std::string> lines = {
            "[0:00:00] 无花果 无花果",
            "[0:00:50] 奇异果 奇异果 奇异果",
            "[0:01:40] 提子 提子 提子 提子"
        };
        
        std::string inputFile = createTestFile(lines);
        std::string outputFile = "data/tests/test_out_15.txt";
        
        HaEngineSSE engine(
            "data/dict/jieba.dict.utf8",
            "data/dict/hmm_model.utf8",
            "data/dict/user.dict.utf8",
            "data/dict/idf.utf8",
            "data/dict/stop_words.utf8",
            60, 10, empty_filter, empty_chooser,
            inputFile, outputFile, 30
        );
        
        // 0-30秒
        engine.cutWord();
        json output1 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output1, "无花果") == 2);
        REQUIRE(getWordCount(output1, "奇异果") == 0);
        
        // 50-80秒
        engine.cutWord();
        json output2 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output2, "奇异果") == 3);
        REQUIRE(getWordCount(output2, "提子") == 0);
        
        // 100-130秒
        engine.cutWord();
        json output3 = readJsonOutput(outputFile);
        REQUIRE(getWordCount(output3, "提子") == 4);
        
        cleanupFile(inputFile);
        cleanupFile(outputFile);
    }
}

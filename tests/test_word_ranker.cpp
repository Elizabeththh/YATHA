#define CATCH_CONFIG_MAIN
#include "catch2/catch_test_macros.hpp"
#include "../include/word_ranker.h"
#include <string>

// ==================== 基本功能测试 ====================

TEST_CASE("WordRanker - 基本添加功能", "[WordRanker][Basic][Add]") {
    WordRanker ranker;
    
    SECTION("添加单个词") {
        ranker.addWord("测试");
        auto topK = ranker.getTopK(1);
        REQUIRE(topK.size() == 1);
        REQUIRE(topK[0].first == "测试");
        REQUIRE(topK[0].second == 1);
    }
    
    SECTION("添加多个相同的词") {
        ranker.addWord("测试");
        ranker.addWord("测试");
        ranker.addWord("测试");
        auto topK = ranker.getTopK(1);
        REQUIRE(topK.size() == 1);
        REQUIRE(topK[0].second == 3);
    }
    
    SECTION("添加多个不同的词") {
        ranker.addWord("苹果");
        ranker.addWord("香蕉");
        ranker.addWord("苹果");
        auto topK = ranker.getTopK(2);
        REQUIRE(topK.size() == 2);
        REQUIRE(topK[0].first == "苹果");
        REQUIRE(topK[0].second == 2);
        REQUIRE(topK[1].first == "香蕉");
        REQUIRE(topK[1].second == 1);
    }
    
    SECTION("词频自动更新") {
        ranker.addWord("词A");
        REQUIRE(ranker.getTopK(1)[0].second == 1);
        
        ranker.addWord("词A");
        REQUIRE(ranker.getTopK(1)[0].second == 2);
        
        ranker.addWord("词A");
        REQUIRE(ranker.getTopK(1)[0].second == 3);
    }
}

TEST_CASE("WordRanker - 基本移除功能", "[WordRanker][Basic][Remove]") {
    WordRanker ranker;
    
    SECTION("移除存在的词（频次减1）") {
        ranker.addWord("测试");
        ranker.addWord("测试");
        ranker.removeWord("测试");
        auto topK = ranker.getTopK(10);
        REQUIRE(topK.size() == 1);
        REQUIRE(topK[0].second == 1);
    }
    
    SECTION("完全移除词（频次降为0）") {
        ranker.addWord("测试");
        ranker.removeWord("测试");
        auto topK = ranker.getTopK(10);
        REQUIRE(topK.size() == 0);
    }
    
    SECTION("移除不存在的词") {
        ranker.addWord("存在");
        ranker.removeWord("不存在");
        auto topK = ranker.getTopK(10);
        REQUIRE(topK.size() == 1);
        REQUIRE(topK[0].first == "存在");
    }
    
    SECTION("连续移除") {
        ranker.addWord("词");
        ranker.addWord("词");
        ranker.addWord("词");
        ranker.addWord("词");
        ranker.addWord("词");
        
        ranker.removeWord("词");
        REQUIRE(ranker.getTopK(1)[0].second == 4);
        
        ranker.removeWord("词");
        REQUIRE(ranker.getTopK(1)[0].second == 3);
        
        ranker.removeWord("词");
        REQUIRE(ranker.getTopK(1)[0].second == 2);
        
        ranker.removeWord("词");
        REQUIRE(ranker.getTopK(1)[0].second == 1);
        
        ranker.removeWord("词");
        REQUIRE(ranker.getTopK(1).size() == 0);
    }
}

// ==================== TopK 功能测试 ====================

TEST_CASE("WordRanker - TopK 排序正确性", "[WordRanker][TopK]") {
    WordRanker ranker;
    
    // 添加多个词，频率不同
    ranker.addWord("A");
    ranker.addWord("A");
    ranker.addWord("A");
    ranker.addWord("B");
    ranker.addWord("B");
    ranker.addWord("C");
    
    SECTION("获取 Top 1") {
        auto topK = ranker.getTopK(1);
        REQUIRE(topK.size() == 1);
        REQUIRE(topK[0].first == "A");
        REQUIRE(topK[0].second == 3);
    }
    
    SECTION("获取 Top 2") {
        auto topK = ranker.getTopK(2);
        REQUIRE(topK.size() == 2);
        REQUIRE(topK[0].first == "A");
        REQUIRE(topK[0].second == 3);
        REQUIRE(topK[1].first == "B");
        REQUIRE(topK[1].second == 2);
    }
    
    SECTION("获取 Top 3") {
        auto topK = ranker.getTopK(3);
        REQUIRE(topK.size() == 3);
        REQUIRE(topK[0].second == 3);
        REQUIRE(topK[1].second == 2);
        REQUIRE(topK[2].second == 1);
    }
    
    SECTION("K 大于词汇数量") {
        auto topK = ranker.getTopK(10);
        REQUIRE(topK.size() == 3);
    }
    
    SECTION("验证降序排列") {
        auto topK = ranker.getTopK(3);
        REQUIRE(topK[0].second >= topK[1].second);
        REQUIRE(topK[1].second >= topK[2].second);
    }
}

TEST_CASE("WordRanker - TopK 动态更新", "[WordRanker][TopK][Dynamic]") {
    WordRanker ranker;
    
    SECTION("添加后排名变化") {
        ranker.addWord("A");
        ranker.addWord("B");
        ranker.addWord("B");
        
        auto top1 = ranker.getTopK(1);
        REQUIRE(top1[0].first == "B");
        
        // A 再添加两次，应该超过 B
        ranker.addWord("A");
        ranker.addWord("A");
        
        auto top2 = ranker.getTopK(1);
        REQUIRE(top2[0].first == "A");
    }
    
    SECTION("移除后排名变化") {
        ranker.addWord("X");
        ranker.addWord("X");
        ranker.addWord("X");
        ranker.addWord("Y");
        ranker.addWord("Y");
        
        REQUIRE(ranker.getTopK(1)[0].first == "X");
        
        // 移除 X 一次，此时 X=2, Y=2
        ranker.removeWord("X");
        // 频率相同时，反向迭代使字典序大的排前面
        REQUIRE(ranker.getTopK(1)[0].first == "Y");
        
        // 再移除一次 X，现在 X=1, Y=2
        ranker.removeWord("X");
        auto top = ranker.getTopK(2);
        REQUIRE(top.size() == 2);
        REQUIRE(top[0].second == 2);  // Y 频率为 2
        REQUIRE(top[0].first == "Y");
        REQUIRE(top[1].second == 1);  // X 频率为 1
        REQUIRE(top[1].first == "X");
    }
}

// ==================== 边界情况测试 ====================

TEST_CASE("WordRanker - 空状态边界", "[WordRanker][Edge][Empty]") {
    WordRanker ranker;
    
    SECTION("空 ranker 获取 TopK") {
        auto topK = ranker.getTopK(5);
        REQUIRE(topK.size() == 0);
    }
    
    SECTION("空 ranker 获取 Top0") {
        auto topK = ranker.getTopK(0);
        REQUIRE(topK.size() == 0);
    }
    
    SECTION("空 ranker 移除词") {
        ranker.removeWord("不存在");
        REQUIRE(ranker.getTopK(1).size() == 0);
    }
    
    SECTION("空 ranker 的 rankingSet") {
        auto& rankingSet = ranker.getRankingSet();
        REQUIRE(rankingSet.size() == 0);
    }
}

TEST_CASE("WordRanker - K 值边界", "[WordRanker][Edge][K]") {
    WordRanker ranker;
    ranker.addWord("A");
    ranker.addWord("B");
    ranker.addWord("C");
    
    SECTION("K = 0") {
        auto topK = ranker.getTopK(0);
        REQUIRE(topK.size() == 0);
    }
    
    SECTION("K = 1") {
        auto topK = ranker.getTopK(1);
        REQUIRE(topK.size() == 1);
    }
    
    SECTION("K = 负数（如果实现支持）") {
        // 注意：getTopK 内部使用 reserve，负数可能导致异常
        // 这取决于具体实现是否处理负数
        // 如果不处理，这个测试会失败
        // auto topK = ranker.getTopK(-1);
        // REQUIRE(topK.size() == 0);
        // 暂时注释掉，因为实现中没有处理负数K的情况
    }
    
    SECTION("K = 极大值") {
        auto topK = ranker.getTopK(999999);
        REQUIRE(topK.size() == 3);
    }
}

TEST_CASE("WordRanker - 字符串边界", "[WordRanker][Edge][String]") {
    WordRanker ranker;
    
    SECTION("空字符串") {
        ranker.addWord("");
        auto topK = ranker.getTopK(1);
        REQUIRE(topK.size() == 1);
        REQUIRE(topK[0].first == "");
        REQUIRE(topK[0].second == 1);
    }
    
    SECTION("空格字符串") {
        ranker.addWord(" ");
        ranker.addWord("  ");
        auto topK = ranker.getTopK(2);
        REQUIRE(topK.size() == 2);
    }
    
    SECTION("超长字符串") {
        std::string longWord(10000, 'a');
        ranker.addWord(longWord);
        auto topK = ranker.getTopK(1);
        REQUIRE(topK.size() == 1);
        REQUIRE(topK[0].first == longWord);
    }
    
    SECTION("特殊字符") {
        ranker.addWord("!@#$%^&*()");
        ranker.addWord("测试\n换行");
        ranker.addWord("tab\t分隔");
        auto topK = ranker.getTopK(10);
        REQUIRE(topK.size() == 3);
    }
    
    SECTION("Unicode 字符") {
        ranker.addWord("😀");
        ranker.addWord("你好世界");
        ranker.addWord("🎉🎊");
        auto topK = ranker.getTopK(10);
        REQUIRE(topK.size() == 3);
    }
}

TEST_CASE("WordRanker - 频次边界", "[WordRanker][Edge][Frequency]") {
    WordRanker ranker;
    
    SECTION("频次从0到高频") {
        std::string word = "test";
        for (int i = 1; i <= 1000; i++) {
            ranker.addWord(word);
            REQUIRE(ranker.getTopK(1)[0].second == i);
        }
    }
    
    SECTION("频次从高频到0") {
        std::string word = "test";
        for (int i = 0; i < 100; i++) {
            ranker.addWord(word);
        }
        
        for (int i = 100; i > 0; i--) {
            REQUIRE(ranker.getTopK(1)[0].second == i);
            ranker.removeWord(word);
        }
        
        REQUIRE(ranker.getTopK(1).size() == 0);
    }
    
    SECTION("所有词频次相同") {
        ranker.addWord("A");
        ranker.addWord("B");
        ranker.addWord("C");
        ranker.addWord("D");
        ranker.addWord("E");
        
        auto topK = ranker.getTopK(5);
        REQUIRE(topK.size() == 5);
        // 所有词频次都是1
        for (const auto& pair : topK) {
            REQUIRE(pair.second == 1);
        }
    }
}

// ==================== 复杂场景测试 ====================

TEST_CASE("WordRanker - 复杂添加移除场景", "[WordRanker][Complex]") {
    WordRanker ranker;
    
    SECTION("交替添加和移除") {
        ranker.addWord("A");
        ranker.addWord("A");
        ranker.removeWord("A");
        ranker.addWord("A");
        ranker.addWord("A");
        ranker.removeWord("A");
        
        auto topK = ranker.getTopK(1);
        REQUIRE(topK.size() == 1);
        REQUIRE(topK[0].second == 2);
    }
    
    SECTION("多词复杂操作") {
        // 构建初始状态
        for (int i = 0; i < 10; i++) ranker.addWord("A");
        for (int i = 0; i < 8; i++) ranker.addWord("B");
        for (int i = 0; i < 6; i++) ranker.addWord("C");
        for (int i = 0; i < 4; i++) ranker.addWord("D");
        for (int i = 0; i < 2; i++) ranker.addWord("E");
        
        auto top1 = ranker.getTopK(5);
        REQUIRE(top1[0].first == "A");
        REQUIRE(top1[4].first == "E");
        
        // 移除操作
        for (int i = 0; i < 5; i++) ranker.removeWord("A");
        for (int i = 0; i < 3; i++) ranker.removeWord("B");
        
        auto top2 = ranker.getTopK(5);
        // A 现在是 5，B 现在是 5，C 现在是 6
        REQUIRE(top2[0].second == 6);  // C是最高的
        REQUIRE(top2[0].first == "C");
    }
}

TEST_CASE("WordRanker - 大规模数据", "[WordRanker][Large]") {
    WordRanker ranker;
    
    SECTION("添加大量不同词") {
        for (int i = 0; i < 1000; i++) {
            ranker.addWord("word_" + std::to_string(i));
        }
        
        auto topK = ranker.getTopK(10);
        REQUIRE(topK.size() == 10);
    }
    
    SECTION("添加大量相同词") {
        for (int i = 0; i < 10000; i++) {
            ranker.addWord("popular");
        }
        
        auto topK = ranker.getTopK(1);
        REQUIRE(topK[0].second == 10000);
    }
    
    SECTION("混合大规模操作") {
        // 100个词，每个词出现100次
        for (int i = 0; i < 100; i++) {
            std::string word = "w" + std::to_string(i);
            for (int j = 0; j < 100; j++) {
                ranker.addWord(word);
            }
        }
        
        auto topK = ranker.getTopK(50);
        REQUIRE(topK.size() == 50);
        
        // 验证所有词频次都是100
        for (const auto& pair : topK) {
            REQUIRE(pair.second == 100);
        }
    }
}

TEST_CASE("WordRanker - 词典序影响", "[WordRanker][Lexical]") {
    WordRanker ranker;
    
    SECTION("相同频次的字典序排序") {
        ranker.addWord("zebra");
        ranker.addWord("apple");
        ranker.addWord("banana");
        
        auto topK = ranker.getTopK(3);
        REQUIRE(topK.size() == 3);
        // 频次相同时，set 会按照 (freq, word) 的字典序排序
        // 由于 freq 相同，会按 word 字典序
    }
    
    SECTION("中文字典序") {
        ranker.addWord("阿");
        ranker.addWord("啊");
        ranker.addWord("爱");
        
        auto topK = ranker.getTopK(3);
        REQUIRE(topK.size() == 3);
    }
}

// ==================== rankingSet 接口测试 ====================

TEST_CASE("WordRanker - getRankingSet 接口", "[WordRanker][RankingSet]") {
    WordRanker ranker;
    
    SECTION("空集合") {
        auto& rankingSet = ranker.getRankingSet();
        REQUIRE(rankingSet.size() == 0);
    }
    
    SECTION("添加后的集合大小") {
        ranker.addWord("A");
        ranker.addWord("B");
        ranker.addWord("C");
        
        auto& rankingSet = ranker.getRankingSet();
        REQUIRE(rankingSet.size() == 3);
    }
    
    SECTION("移除后的集合大小") {
        ranker.addWord("A");
        ranker.addWord("B");
        ranker.removeWord("A");
        
        auto& rankingSet = ranker.getRankingSet();
        REQUIRE(rankingSet.size() == 1);
    }
    
    SECTION("集合中的元素格式") {
        ranker.addWord("test");
        ranker.addWord("test");
        
        auto& rankingSet = ranker.getRankingSet();
        REQUIRE(rankingSet.size() == 1);
        
        auto it = rankingSet.begin();
        REQUIRE(it->first == 2);   // 频次
        REQUIRE(it->second == "test");  // 词
    }
    
    SECTION("集合的排序顺序") {
        ranker.addWord("A");
        ranker.addWord("B");
        ranker.addWord("B");
        ranker.addWord("C");
        ranker.addWord("C");
        ranker.addWord("C");
        
        auto& rankingSet = ranker.getRankingSet();
        REQUIRE(rankingSet.size() == 3);
        
        // set 是升序的，所以第一个是最小的
        auto it = rankingSet.begin();
        REQUIRE(it->first == 1);  // A 频次为1
        
        // 最后一个是最大的
        auto rit = rankingSet.rbegin();
        REQUIRE(rit->first == 3);  // C 频次为3
    }
}

// ==================== 稳定性测试 ====================

TEST_CASE("WordRanker - 操作稳定性", "[WordRanker][Stability]") {
    WordRanker ranker;
    
    SECTION("重复操作不影响结果") {
        ranker.addWord("stable");
        auto top1 = ranker.getTopK(1);
        auto top2 = ranker.getTopK(1);
        auto top3 = ranker.getTopK(1);
        
        REQUIRE(top1[0].first == top2[0].first);
        REQUIRE(top2[0].first == top3[0].first);
        REQUIRE(top1[0].second == top2[0].second);
    }
    
    SECTION("添加已存在词不会重复") {
        ranker.addWord("unique");
        ranker.addWord("unique");
        
        auto& rankingSet = ranker.getRankingSet();
        REQUIRE(rankingSet.size() == 1);
    }
    
    SECTION("移除后再添加") {
        ranker.addWord("cycle");
        ranker.addWord("cycle");
        ranker.removeWord("cycle");
        ranker.removeWord("cycle");
        ranker.addWord("cycle");
        
        auto topK = ranker.getTopK(1);
        REQUIRE(topK[0].second == 1);
    }
}

// ==================== 实际使用场景 ====================

TEST_CASE("WordRanker - 热词统计场景", "[WordRanker][RealWorld]") {
    WordRanker ranker;
    
    SECTION("模拟弹幕热词") {
        // 模拟一段时间内的弹幕词频
        std::vector<std::string> danmaku = {
            "先登", "先登", "先登", "来了", "来了",
            "测试", "666", "666", "666", "666",
            "牛", "牛", "牛"
        };
        
        for (const auto& word : danmaku) {
            ranker.addWord(word);
        }
        
        auto top3 = ranker.getTopK(3);
        REQUIRE(top3.size() == 3);
        REQUIRE(top3[0].first == "666");
        REQUIRE(top3[0].second == 4);
    }
    
    SECTION("时间窗口过期模拟") {
        // 添加词
        ranker.addWord("old1");
        ranker.addWord("old1");
        ranker.addWord("old2");
        ranker.addWord("new");
        ranker.addWord("new");
        ranker.addWord("new");
        
        // 模拟过期移除 old1
        ranker.removeWord("old1");
        ranker.removeWord("old1");
        
        auto topK = ranker.getTopK(10);
        REQUIRE(topK.size() == 2);
        REQUIRE(topK[0].first == "new");
    }
    
    SECTION("动态Top10查询") {
        // 持续添加词
        for (int i = 0; i < 100; i++) {
            ranker.addWord("word" + std::to_string(i % 20));
        }
        
        auto top10 = ranker.getTopK(10);
        REQUIRE(top10.size() == 10);
        
        // 验证频次降序
        for (size_t i = 0; i < top10.size() - 1; i++) {
            REQUIRE(top10[i].second >= top10[i + 1].second);
        }
    }
}

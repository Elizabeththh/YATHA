#define CATCH_CONFIG_MAIN
#include "catch2/catch_test_macros.hpp"
#include "../include/time_window_manager.h"

// ==================== 基础功能测试 ====================

TEST_CASE("TimeWindowManager - 初始状态", "[TimeWindowManager][Basic]") {
    TimeWindowManager manager(60);
    
    SECTION("初始值验证") {
        REQUIRE(manager.getCurrentTime() == -1);
        REQUIRE(manager.getCurrentWindowSize() == 0);
        REQUIRE(manager.isEmpty() == true);
    }
}

TEST_CASE("TimeWindowManager - 添加词汇基本功能", "[TimeWindowManager][AddWord]") {
    TimeWindowManager manager(60);
    
    SECTION("添加第一个词") {
        manager.addWord(10, "测试");
        REQUIRE(manager.isEmpty() == false);
    }
    
    SECTION("添加多个词到队列") {
        manager.addWord(10, "词1");
        manager.addWord(10, "词2");
        manager.addWord(10, "词3");
        REQUIRE(manager.isEmpty() == false);
    }
}

// ==================== 窗口计数逻辑测试 ====================

TEST_CASE("TimeWindowManager - 窗口计数机制", "[TimeWindowManager][WindowCount]") {
    TimeWindowManager manager(3);  // 窗口大小为3
    
    SECTION("窗口未满时不触发移除") {
        // 第1个时间点
        REQUIRE(manager.shouldRemoveOld(10) == false);
        REQUIRE(manager.getCurrentTime() == 10);
        REQUIRE(manager.getCurrentWindowSize() == 1);
        
        // 第2个时间点
        REQUIRE(manager.shouldRemoveOld(20) == false);
        REQUIRE(manager.getCurrentTime() == 20);
        REQUIRE(manager.getCurrentWindowSize() == 2);
        
        // 第3个时间点
        REQUIRE(manager.shouldRemoveOld(30) == false);
        REQUIRE(manager.getCurrentTime() == 30);
        REQUIRE(manager.getCurrentWindowSize() == 3);
    }
    
    SECTION("窗口满后触发移除") {
        // 填满窗口
        manager.shouldRemoveOld(10);
        manager.shouldRemoveOld(20);
        manager.shouldRemoveOld(30);
        
        // 第4个时间点，窗口已满，应该触发移除
        REQUIRE(manager.shouldRemoveOld(40) == true);
        REQUIRE(manager.getCurrentTime() == 40);
        REQUIRE(manager.getCurrentWindowSize() == 3); // 计数保持不变
    }
    
    SECTION("相同时间戳不增加计数") {
        REQUIRE(manager.shouldRemoveOld(10) == false);
        REQUIRE(manager.getCurrentWindowSize() == 1);
        
        // 相同时间，不增加计数
        REQUIRE(manager.shouldRemoveOld(10) == false);
        REQUIRE(manager.getCurrentWindowSize() == 1);
        
        REQUIRE(manager.shouldRemoveOld(10) == false);
        REQUIRE(manager.getCurrentWindowSize() == 1);
    }
}

// ==================== 时间阈值移除测试 ====================

TEST_CASE("TimeWindowManager - 基于时间的词汇移除", "[TimeWindowManager][TimeThreshold]") {
    TimeWindowManager manager(60);  // 时间窗口60秒
    
    SECTION("基本时间阈值移除") {
        // 添加词汇
        manager.addWord(10, "词1");
        manager.addWord(20, "词2");
        manager.addWord(30, "词3");
        manager.addWord(80, "词4");
        
        // 更新时间到80秒
        manager.shouldRemoveOld(10);
        manager.shouldRemoveOld(20);
        manager.shouldRemoveOld(30);
        manager.shouldRemoveOld(80);
        
        // 阈值 = 80 - 60 = 20，应该移除时间戳 <= 20 的词
        auto outdated = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated.size() == 2);
        REQUIRE(outdated[0].first == 10);
        REQUIRE(outdated[0].second == "词1");
        REQUIRE(outdated[1].first == 20);
        REQUIRE(outdated[1].second == "词2");
    }
    
    SECTION("精确边界测试") {
        manager.addWord(0, "词A");
        manager.addWord(59, "词B");
        manager.addWord(60, "词C");
        manager.addWord(61, "词D");
        
        manager.shouldRemoveOld(0);
        manager.shouldRemoveOld(59);
        manager.shouldRemoveOld(60);
        manager.shouldRemoveOld(61);
        
        // 更新到时间120
        manager.shouldRemoveOld(120);
        
        // 阈值 = 120 - 60 = 60，应该移除时间戳 <= 60 的词
        auto outdated = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated.size() == 3);
        REQUIRE(outdated[0].second == "词A");
        REQUIRE(outdated[1].second == "词B");
        REQUIRE(outdated[2].second == "词C");
    }
}

// ==================== 窗口填充与移除组合测试 ====================

TEST_CASE("TimeWindowManager - 窗口填充到移除的完整流程", "[TimeWindowManager][FullCycle]") {
    TimeWindowManager manager(5);  // 5秒窗口
    
    SECTION("逐步填充后开始移除") {
        // 时间0-4秒，窗口逐步填充
        manager.addWord(0, "t0");
        REQUIRE(manager.shouldRemoveOld(0) == false);
        
        manager.addWord(1, "t1");
        REQUIRE(manager.shouldRemoveOld(1) == false);
        
        manager.addWord(2, "t2");
        REQUIRE(manager.shouldRemoveOld(2) == false);
        
        manager.addWord(3, "t3");
        REQUIRE(manager.shouldRemoveOld(3) == false);
        
        manager.addWord(4, "t4");
        REQUIRE(manager.shouldRemoveOld(4) == false);
        
        REQUIRE(manager.getCurrentWindowSize() == 5);
        
        // 时间5秒，窗口满了，触发移除
        manager.addWord(5, "t5");
        REQUIRE(manager.shouldRemoveOld(5) == true);
        
        // 阈值 = 5 - 5 = 0，移除时间戳 <= 0 的词
        auto outdated = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated.size() == 1);
        REQUIRE(outdated[0].first == 0);
        REQUIRE(outdated[0].second == "t0");
    }
    
    SECTION("持续添加和移除") {
        // 填满窗口
        for (int i = 0; i < 5; i++) {
            manager.addWord(i, "word" + std::to_string(i));
            manager.shouldRemoveOld(i);
        }
        
        // 继续添加，每次都会移除
        manager.addWord(10, "w10a");
        manager.addWord(10, "w10b");
        REQUIRE(manager.shouldRemoveOld(10) == true);
        
        // 阈值 = 10 - 5 = 5，移除时间戳 <= 5 的词
        auto outdated = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated.size() == 5);  // 移除0-4的所有词
    }
}

// ==================== 多词汇同时间戳测试 ====================

TEST_CASE("TimeWindowManager - 同一时间戳多个词汇", "[TimeWindowManager][MultipleWords]") {
    TimeWindowManager manager(10);
    
    SECTION("同一时间戳添加多个词") {
        manager.addWord(10, "词A");
        manager.addWord(10, "词B");
        manager.addWord(10, "词C");
        
        manager.shouldRemoveOld(10);
        REQUIRE(manager.getCurrentWindowSize() == 1);
        
        manager.addWord(20, "词D");
        manager.addWord(20, "词E");
        
        manager.shouldRemoveOld(20);
        REQUIRE(manager.getCurrentWindowSize() == 2);
    }
    
    SECTION("同时间戳词汇的批量移除") {
        // 时间0添加3个词
        manager.addWord(0, "a1");
        manager.addWord(0, "a2");
        manager.addWord(0, "a3");
        manager.shouldRemoveOld(0);
        
        // 时间5添加2个词
        manager.addWord(5, "b1");
        manager.addWord(5, "b2");
        manager.shouldRemoveOld(5);
        
        // 填充到窗口满
        for (int i = 1; i <= 8; i++) {
            manager.shouldRemoveOld(i);
        }
        
        // 时间15，阈值 = 15 - 10 = 5
        manager.shouldRemoveOld(15);
        auto outdated = manager.getAndRemoveOutdatedWords();
        
        // 应该移除所有时间戳 <= 5 的词
        REQUIRE(outdated.size() == 5);
    }
}

// ==================== 边界条件测试 ====================

TEST_CASE("TimeWindowManager - 边界条件", "[TimeWindowManager][Edge]") {
    SECTION("窗口大小为1") {
        TimeWindowManager manager(1);
        
        manager.addWord(10, "词1");
        REQUIRE(manager.shouldRemoveOld(10) == false);
        REQUIRE(manager.getCurrentWindowSize() == 1);
        
        // 窗口满了
        manager.addWord(20, "词2");
        REQUIRE(manager.shouldRemoveOld(20) == true);
        
        // 阈值 = 20 - 1 = 19，移除 <= 19 的词
        auto outdated = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated.size() == 1);
        REQUIRE(outdated[0].first == 10);
    }
    
    SECTION("时间为0") {
        TimeWindowManager manager(60);
        
        manager.addWord(0, "词0");
        REQUIRE(manager.shouldRemoveOld(0) == false);
        REQUIRE(manager.getCurrentTime() == 0);
    }
    
    SECTION("时间倒退不会发生（假设输入递增）") {
        TimeWindowManager manager(60);
        
        manager.shouldRemoveOld(100);
        REQUIRE(manager.getCurrentTime() == 100);
        
        // 如果时间相同，不会改变状态
        manager.shouldRemoveOld(100);
        REQUIRE(manager.getCurrentTime() == 100);
    }
}

// ==================== 大规模数据测试 ====================

TEST_CASE("TimeWindowManager - 大量词汇处理", "[TimeWindowManager][Large]") {
    TimeWindowManager manager(100);
    
    SECTION("大量词汇添加") {
        // 添加1000个词
        for (int i = 0; i < 1000; i++) {
            manager.addWord(i % 100, "word" + std::to_string(i));
        }
        
        REQUIRE(manager.isEmpty() == false);
    }
    
    SECTION("批量移除测试") {
        // 时间0-99添加词汇
        for (int t = 0; t < 100; t++) {
            manager.addWord(t, "t" + std::to_string(t));
            manager.shouldRemoveOld(t);
        }
        
        // 时间200，阈值 = 200 - 100 = 100
        manager.shouldRemoveOld(200);
        auto outdated = manager.getAndRemoveOutdatedWords();
        
        // 应该移除所有时间戳 <= 100 的词（即0-99的词）
        REQUIRE(outdated.size() == 100);
    }
}

// ==================== 实际使用场景测试 ====================

TEST_CASE("TimeWindowManager - 实际热词统计场景", "[TimeWindowManager][RealWorld]") {
    TimeWindowManager manager(3600);  // 1小时窗口
    
    SECTION("模拟弹幕热词统计") {
        // 0:00:00 - 添加一些词
        manager.addWord(0, "先登");
        manager.addWord(0, "吃什么");
        manager.shouldRemoveOld(0);
        
        // 0:00:30
        manager.addWord(30, "先登");
        manager.addWord(30, "来了");
        manager.shouldRemoveOld(30);
        
        // 0:10:00
        manager.addWord(600, "测试");
        manager.shouldRemoveOld(600);
        
        // 1:00:00 - 窗口内
        manager.addWord(3600, "一小时");
        manager.shouldRemoveOld(3600);
        
        REQUIRE(manager.getCurrentTime() == 3600);
        REQUIRE(manager.getCurrentWindowSize() == 4);
        
        // 1:00:01 - 触发移除
        manager.shouldRemoveOld(3601);
        auto outdated = manager.getAndRemoveOutdatedWords();
        
        // 阈值 = 3601 - 3600 = 1，移除时间戳 <= 1 的词
        REQUIRE(outdated.size() == 2);
    }
    
    SECTION("跨时间段查询") {
        // 填充窗口到满（每分钟一次，共60次）
        for (int i = 0; i < 60; i++) {
            manager.addWord(i * 60, "word" + std::to_string(i));
            manager.shouldRemoveOld(i * 60);
        }
        
        // 窗口满了，maxWindowSize不是时间点数量限制
        // curWindowSize会停在60（因为每分钟都是新时间点）
        REQUIRE(manager.getCurrentWindowSize() == 60);
        
        // 再添加新时间点会触发移除
        manager.shouldRemoveOld(7200);
        REQUIRE(manager.shouldRemoveOld(7200) == false); // 时间相同，不触发
        
        auto outdated = manager.getAndRemoveOutdatedWords();
        
        // 阈值 = 7200 - 3600 = 3600
        // 应该移除所有 <= 3600 的词
        REQUIRE(outdated.size() >= 60);  // 至少移除前60个
    }
}

// ==================== 空队列测试 ====================

TEST_CASE("TimeWindowManager - 空队列操作", "[TimeWindowManager][Empty]") {
    TimeWindowManager manager(60);
    
    SECTION("空队列调用getAndRemoveOutdatedWords") {
        auto outdated = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated.size() == 0);
        REQUIRE(manager.isEmpty() == true);
    }
    
    SECTION("更新时间但不添加词汇") {
        manager.shouldRemoveOld(10);
        manager.shouldRemoveOld(20);
        manager.shouldRemoveOld(30);
        
        REQUIRE(manager.getCurrentTime() == 30);
        REQUIRE(manager.getCurrentWindowSize() == 3);
        REQUIRE(manager.isEmpty() == true);
        
        auto outdated = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated.size() == 0);
    }
}

// ==================== 连续移除测试 ====================

TEST_CASE("TimeWindowManager - 连续移除操作", "[TimeWindowManager][ContinuousRemoval]") {
    TimeWindowManager manager(10);
    
    SECTION("多次连续移除") {
        // 添加词汇 0-9秒
        for (int i = 0; i < 10; i++) {
            manager.addWord(i, "w" + std::to_string(i));
            manager.shouldRemoveOld(i);
        }
        
        // 第一次移除，时间15
        manager.shouldRemoveOld(15);
        auto outdated1 = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated1.size() == 6);  // 0-5
        
        // 第二次移除，时间20
        manager.shouldRemoveOld(20);
        auto outdated2 = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated2.size() == 4);  // 6-9
        
        // 第三次移除，队列已空
        manager.shouldRemoveOld(25);
        auto outdated3 = manager.getAndRemoveOutdatedWords();
        REQUIRE(outdated3.size() == 0);
    }
}

// ==================== 时间窗口滑动测试 ====================

TEST_CASE("TimeWindowManager - 滑动窗口效果", "[TimeWindowManager][SlidingWindow]") {
    TimeWindowManager manager(5);
    
    SECTION("窗口滑动验证") {
        // 初始：添加0-4秒的词
        for (int i = 0; i < 5; i++) {
            manager.addWord(i, "initial" + std::to_string(i));
            manager.shouldRemoveOld(i);
        }
        
        // 时间5：应该移除时间0的词
        manager.addWord(5, "new5");
        manager.shouldRemoveOld(5);
        auto r1 = manager.getAndRemoveOutdatedWords();
        REQUIRE(r1.size() == 1);
        REQUIRE(r1[0].first == 0);
        
        // 时间6：应该移除时间1的词
        manager.addWord(6, "new6");
        manager.shouldRemoveOld(6);
        auto r2 = manager.getAndRemoveOutdatedWords();
        REQUIRE(r2.size() == 1);
        REQUIRE(r2[0].first == 1);
        
        // 时间10：应该移除时间2-5的词
        manager.shouldRemoveOld(10);
        auto r3 = manager.getAndRemoveOutdatedWords();
        REQUIRE(r3.size() == 4);  // 2, 3, 4, 5
    }
}

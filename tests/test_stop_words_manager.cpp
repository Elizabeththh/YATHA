#define CATCH_CONFIG_MAIN
#include "catch2/catch_test_macros.hpp"
#include "../include/stop_words_manager.h"

// ==================== 停用词测试 ====================

TEST_CASE("停用词 - 中文标点符号", "[StopWords][Punctuation][Chinese]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("常见中文标点") {
        std::string comma = "，";
        std::string period = "。";
        std::string question = "？";
        std::string exclamation = "！";
        std::string colon = "：";
        std::string semicolon = "；";
        std::string pause = "、";
        std::string ellipsis = "…";
        
        REQUIRE(manager.isStopWord(comma) == true);
        REQUIRE(manager.isStopWord(period) == true);
        REQUIRE(manager.isStopWord(question) == true);
        REQUIRE(manager.isStopWord(exclamation) == true);
        REQUIRE(manager.isStopWord(colon) == true);
        REQUIRE(manager.isStopWord(semicolon) == true);
        REQUIRE(manager.isStopWord(pause) == true);
        REQUIRE(manager.isStopWord(ellipsis) == true);
    }
    
    SECTION("中文引号") {
        std::string leftQuote1 = "\u201c";   // "
        std::string rightQuote1 = "\u201d";  // "
        std::string leftQuote2 = "\u2018";   // '
        std::string rightQuote2 = "\u2019";  // '
        std::string leftBook = "\u300a";     // 《
        std::string rightBook = "\u300b";    // 》
        
        REQUIRE(manager.isStopWord(leftQuote1) == true);
        REQUIRE(manager.isStopWord(rightQuote1) == true);
        REQUIRE(manager.isStopWord(leftQuote2) == true);
        REQUIRE(manager.isStopWord(rightQuote2) == true);
        REQUIRE(manager.isStopWord(leftBook) == true);
        REQUIRE(manager.isStopWord(rightBook) == true);
    }
    
    SECTION("中文括号") {
        std::string leftParen1 = "（";
        std::string rightParen1 = "）";
        std::string leftBracket1 = "【";
        std::string rightBracket1 = "】";
        std::string leftBracket2 = "「";
        std::string rightBracket2 = "」";
        
        REQUIRE(manager.isStopWord(leftParen1) == true);
        REQUIRE(manager.isStopWord(rightParen1) == true);
        REQUIRE(manager.isStopWord(leftBracket1) == true);
        REQUIRE(manager.isStopWord(rightBracket1) == true);
        REQUIRE(manager.isStopWord(leftBracket2) == true);
        REQUIRE(manager.isStopWord(rightBracket2) == true);
    }
}

TEST_CASE("停用词 - 英文标点符号", "[StopWords][Punctuation][English]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("基本英文标点") {
        std::string period = ".";
        std::string comma = ",";
        std::string question = "?";
        std::string exclamation = "!";
        std::string colon = ":";
        std::string semicolon = ";";
        
        REQUIRE(manager.isStopWord(period) == true);
        REQUIRE(manager.isStopWord(comma) == true);
        REQUIRE(manager.isStopWord(question) == true);
        REQUIRE(manager.isStopWord(exclamation) == true);
        REQUIRE(manager.isStopWord(colon) == true);
        REQUIRE(manager.isStopWord(semicolon) == true);
    }
    
    SECTION("英文引号和括号") {
        std::string doubleQuote = "\"";
        std::string leftParen = "(";
        std::string rightParen = ")";
        std::string leftBracket = "[";
        std::string rightBracket = "]";
        std::string leftBrace = "{";
        std::string rightBrace = "}";
        
        REQUIRE(manager.isStopWord(doubleQuote) == true);
        REQUIRE(manager.isStopWord(leftParen) == true);
        REQUIRE(manager.isStopWord(rightParen) == true);
        REQUIRE(manager.isStopWord(leftBracket) == true);
        REQUIRE(manager.isStopWord(rightBracket) == true);
        REQUIRE(manager.isStopWord(leftBrace) == true);
        REQUIRE(manager.isStopWord(rightBrace) == true);
    }
    
    SECTION("其他英文符号") {
        std::string dash = "-";
        std::string underscore = "_";
        std::string slash = "/";
        std::string backslash = "\\";
        std::string at = "@";
        std::string hash = "#";
        std::string dollar = "$";
        std::string percent = "%";
        std::string ampersand = "&";
        std::string asterisk = "*";
        
        REQUIRE(manager.isStopWord(dash) == true);
        REQUIRE(manager.isStopWord(underscore) == true);
        REQUIRE(manager.isStopWord(slash) == true);
        REQUIRE(manager.isStopWord(backslash) == true);
        REQUIRE(manager.isStopWord(at) == true);
        REQUIRE(manager.isStopWord(hash) == true);
        REQUIRE(manager.isStopWord(dollar) == true);
        REQUIRE(manager.isStopWord(percent) == true);
        REQUIRE(manager.isStopWord(ampersand) == true);
        REQUIRE(manager.isStopWord(asterisk) == true);
    }
}

TEST_CASE("停用词 - 人称代词", "[StopWords][Pronoun]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("第一人称") {
        std::string wo = "我";
        std::string women = "我们";
        std::string zan = "咱";
        std::string zanmen = "咱们";
        std::string an = "俺";
        std::string anmen = "俺们";
        
        REQUIRE(manager.isStopWord(wo) == true);
        REQUIRE(manager.isStopWord(women) == true);
        REQUIRE(manager.isStopWord(zan) == true);
        REQUIRE(manager.isStopWord(zanmen) == true);
        REQUIRE(manager.isStopWord(an) == true);
        REQUIRE(manager.isStopWord(anmen) == true);
    }
    
    SECTION("第二人称") {
        std::string ni = "你";
        std::string nimen = "你们";
        std::string nin = "您";
        
        REQUIRE(manager.isStopWord(ni) == true);
        REQUIRE(manager.isStopWord(nimen) == true);
        REQUIRE(manager.isStopWord(nin) == true);
    }
    
    SECTION("第三人称") {
        std::string ta1 = "他";
        std::string tamen1 = "他们";
        std::string ta2 = "她";
        std::string tamen2 = "她们";
        std::string ta3 = "它";
        std::string tamen3 = "它们";
        
        REQUIRE(manager.isStopWord(ta1) == true);
        REQUIRE(manager.isStopWord(tamen1) == true);
        REQUIRE(manager.isStopWord(ta2) == true);
        REQUIRE(manager.isStopWord(tamen2) == true);
        REQUIRE(manager.isStopWord(ta3) == true);
        REQUIRE(manager.isStopWord(tamen3) == true);
    }
    
    SECTION("其他人称") {
        std::string dajia = "大家";
        std::string ren = "人";
        std::string renmen = "人们";
        std::string renjia = "人家";
        std::string taren = "他人";
        std::string bieren = "别人";
        
        REQUIRE(manager.isStopWord(dajia) == true);
        REQUIRE(manager.isStopWord(ren) == true);
        REQUIRE(manager.isStopWord(renmen) == true);
        REQUIRE(manager.isStopWord(renjia) == true);
        REQUIRE(manager.isStopWord(taren) == true);
        REQUIRE(manager.isStopWord(bieren) == true);
    }
}

TEST_CASE("停用词 - 语气词", "[StopWords][Interjection]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("常见语气词") {
        std::string ba = "吧";
        std::string ne = "呢";
        std::string a = "啊";
        std::string ya = "呀";
        std::string ma = "嘛";
        std::string o = "哦";
        std::string wa = "哇";
        
        REQUIRE(manager.isStopWord(ba) == true);
        REQUIRE(manager.isStopWord(ne) == true);
        REQUIRE(manager.isStopWord(a) == true);
        REQUIRE(manager.isStopWord(ya) == true);
        REQUIRE(manager.isStopWord(ma) == true);
        REQUIRE(manager.isStopWord(o) == true);
        REQUIRE(manager.isStopWord(wa) == true);
    }
    
    SECTION("感叹语气") {
        std::string ai = "哎";
        std::string aiya = "哎呀";
        std::string aiyo = "哎哟";
        std::string haha = "哈哈";
        std::string hehe = "呵呵";
        std::string heihei = "嘿嘿";
        std::string wuhu = "呜呼";
        
        REQUIRE(manager.isStopWord(ai) == true);
        REQUIRE(manager.isStopWord(aiya) == true);
        REQUIRE(manager.isStopWord(aiyo) == true);
        REQUIRE(manager.isStopWord(haha) == true);
        REQUIRE(manager.isStopWord(hehe) == true);
        REQUIRE(manager.isStopWord(heihei) == true);
        REQUIRE(manager.isStopWord(wuhu) == true);
    }
    
    SECTION("拟声词") {
        std::string dingdong = "叮咚";
        std::string peng = "砰";
        std::string dong = "咚";
        std::string xi = "嘻";
        std::string en = "嗯";
        std::string weng = "嗡";
        
        REQUIRE(manager.isStopWord(dingdong) == true);
        REQUIRE(manager.isStopWord(dong) == true);
        REQUIRE(manager.isStopWord(xi) == true);
        REQUIRE(manager.isStopWord(en) == true);
        REQUIRE(manager.isStopWord(weng) == true);
    }
}

TEST_CASE("停用词 - 疑问词", "[StopWords][Question]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("疑问代词") {
        std::string shenme = "什么";
        std::string shenmeyang = "什么样";
        std::string na = "哪";
        std::string nage = "哪个";
        std::string naxie = "哪些";
        std::string nar = "哪儿";
        std::string nali = "哪里";
        std::string shei = "谁";
        
        REQUIRE(manager.isStopWord(shenme) == true);
        REQUIRE(manager.isStopWord(shenmeyang) == true);
        REQUIRE(manager.isStopWord(na) == true);
        REQUIRE(manager.isStopWord(nage) == true);
        REQUIRE(manager.isStopWord(naxie) == true);
        REQUIRE(manager.isStopWord(nar) == true);
        REQUIRE(manager.isStopWord(nali) == true);
        REQUIRE(manager.isStopWord(shei) == true);
    }
    
    SECTION("疑问副词") {
        std::string weishenme = "为什么";
        std::string weihe = "为何";
        std::string heyi = "何以";
        std::string heshi = "何时";
        std::string hechu = "何处";
        std::string zenme = "怎么";
        std::string zenmeyang = "怎么样";
        std::string duoshao = "多少";
        
        REQUIRE(manager.isStopWord(weishenme) == true);
        REQUIRE(manager.isStopWord(weihe) == true);
        REQUIRE(manager.isStopWord(heyi) == true);
        REQUIRE(manager.isStopWord(heshi) == true);
        REQUIRE(manager.isStopWord(hechu) == true);
        REQUIRE(manager.isStopWord(zenme) == true);
        REQUIRE(manager.isStopWord(zenmeyang) == true);
        REQUIRE(manager.isStopWord(duoshao) == true);
    }
    
    SECTION("疑问语气词") {
        std::string ma = "吗";
        std::string ne = "呢";
        std::string ba = "吧";
        
        REQUIRE(manager.isStopWord(ma) == true);
        REQUIRE(manager.isStopWord(ne) == true);
        REQUIRE(manager.isStopWord(ba) == true);
    }
}

TEST_CASE("停用词 - 连词", "[StopWords][Conjunction]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("并列连词") {
        std::string he = "和";
        std::string yu = "与";
        std::string ji = "及";
        std::string yiji = "以及";
        std::string haiyou = "还有";
        std::string erqie = "而且";
        std::string bingqie = "并且";
        
        REQUIRE(manager.isStopWord(he) == true);
        REQUIRE(manager.isStopWord(yu) == true);
        REQUIRE(manager.isStopWord(ji) == true);
        REQUIRE(manager.isStopWord(yiji) == true);
        REQUIRE(manager.isStopWord(haiyou) == true);
        REQUIRE(manager.isStopWord(erqie) == true);
        REQUIRE(manager.isStopWord(bingqie) == true);
    }
    
    SECTION("转折连词") {
        std::string danshi = "但是";
        std::string keshi = "可是";
        std::string que = "却";
        std::string raner = "然而";
        std::string buran = "不然";
        std::string buguo = "不过";
        std::string zhishi = "只是";
        
        REQUIRE(manager.isStopWord(danshi) == true);
        REQUIRE(manager.isStopWord(keshi) == true);
        REQUIRE(manager.isStopWord(que) == true);
        REQUIRE(manager.isStopWord(raner) == true);
        REQUIRE(manager.isStopWord(buran) == true);
        REQUIRE(manager.isStopWord(buguo) == true);
        REQUIRE(manager.isStopWord(zhishi) == true);
    }
    
    SECTION("因果连词") {
        std::string yinwei = "因为";
        std::string yinci = "因此";
        std::string suoyi = "所以";
        std::string guer = "故而";
        std::string yushi = "于是";
        std::string conger = "从而";
        
        REQUIRE(manager.isStopWord(yinwei) == true);
        REQUIRE(manager.isStopWord(yinci) == true);
        REQUIRE(manager.isStopWord(suoyi) == true);
        REQUIRE(manager.isStopWord(guer) == true);
        REQUIRE(manager.isStopWord(yushi) == true);
        REQUIRE(manager.isStopWord(conger) == true);
    }
    
    SECTION("条件连词") {
        std::string ruguo = "如果";
        std::string jiaru = "假如";
        std::string yaoshi = "要是";
        std::string tangruo = "倘若";
        std::string jishi = "即使";
        std::string jiran = "既然";
        std::string zhiyao = "只要";
        std::string wulun = "无论";
        
        REQUIRE(manager.isStopWord(ruguo) == true);
        REQUIRE(manager.isStopWord(jiaru) == true);
        REQUIRE(manager.isStopWord(yaoshi) == true);
        REQUIRE(manager.isStopWord(tangruo) == true);
        REQUIRE(manager.isStopWord(jishi) == true);
        REQUIRE(manager.isStopWord(jiran) == true);
        REQUIRE(manager.isStopWord(zhiyao) == true);
        REQUIRE(manager.isStopWord(wulun) == true);
    }
}

TEST_CASE("停用词 - 介词", "[StopWords][Preposition]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("常见介词") {
        std::string zai = "在";
        std::string yu = "于";
        std::string cong = "从";
        std::string xiang = "向";
        std::string dao = "到";
        std::string yi = "以";
        std::string wei = "为";
        std::string dui = "对";
        std::string guanyu = "关于";
        std::string duiyu = "对于";
        std::string yinyu = "因于";
        
        REQUIRE(manager.isStopWord(zai) == true);
        REQUIRE(manager.isStopWord(yu) == true);
        REQUIRE(manager.isStopWord(cong) == true);
        REQUIRE(manager.isStopWord(xiang) == true);
        REQUIRE(manager.isStopWord(dao) == true);
        REQUIRE(manager.isStopWord(yi) == true);
        REQUIRE(manager.isStopWord(wei) == true);
        REQUIRE(manager.isStopWord(dui) == true);
        REQUIRE(manager.isStopWord(guanyu) == true);
        REQUIRE(manager.isStopWord(duiyu) == true);
    }
}

TEST_CASE("停用词 - 助词", "[StopWords][Auxiliary]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("结构助词") {
        std::string de = "的";
        std::string di = "地";
        std::string dei = "得";
        std::string zhi = "之";
        
        REQUIRE(manager.isStopWord(de) == true);
        REQUIRE(manager.isStopWord(di) == true);
        REQUIRE(manager.isStopWord(dei) == true);
        REQUIRE(manager.isStopWord(zhi) == true);
    }
    
    SECTION("动态助词") {
        std::string le = "了";
        std::string zhe = "着";
        std::string guo = "过";
        
        REQUIRE(manager.isStopWord(le) == true);
        REQUIRE(manager.isStopWord(zhe) == true);
        REQUIRE(manager.isStopWord(guo) == true);
    }
}

TEST_CASE("停用词 - 副词", "[StopWords][Adverb]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("程度副词") {
        std::string hen = "很";
        std::string zui = "最";
        
        REQUIRE(manager.isStopWord(hen) == true);
        REQUIRE(manager.isStopWord(zui) == true);
    }
    
    SECTION("否定副词") {
        std::string bu = "不";
        std::string mei = "没";
        std::string wu = "无";
        std::string bie = "别";
        std::string feidang = "非但";  // 非但是停用词
        
        REQUIRE(manager.isStopWord(bu) == true);
        REQUIRE(manager.isStopWord(mei) == true);
        REQUIRE(manager.isStopWord(wu) == true);
        REQUIRE(manager.isStopWord(bie) == true);
        REQUIRE(manager.isStopWord(feidang) == true);
    }
    
    SECTION("时间副词") {
        std::string zhengzhi = "正值";  // 正值是停用词
        std::string zai = "在";
        std::string cai = "才";
        std::string yi = "已";
        std::string congci = "从此";
        
        REQUIRE(manager.isStopWord(zhengzhi) == true);
        REQUIRE(manager.isStopWord(zai) == true);
        REQUIRE(manager.isStopWord(cai) == true);
        REQUIRE(manager.isStopWord(yi) == true);
        REQUIRE(manager.isStopWord(congci) == true);
    }
}

TEST_CASE("停用词 - 量词和指示词", "[StopWords][Measure]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("常见量词和指示词") {
        std::string ge = "个";
        std::string xie = "些";
        std::string gebie = "个别";
        std::string yixie = "一些";
        
        REQUIRE(manager.isStopWord(ge) == true);
        REQUIRE(manager.isStopWord(xie) == true);
        REQUIRE(manager.isStopWord(gebie) == true);
        REQUIRE(manager.isStopWord(yixie) == true);
    }
}

TEST_CASE("停用词 - 数字", "[StopWords][Number]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("阿拉伯数字") {
        std::string zero = "0";
        std::string one = "1";
        std::string two = "2";
        std::string three = "3";
        std::string nine = "9";
        
        REQUIRE(manager.isStopWord(zero) == true);
        REQUIRE(manager.isStopWord(one) == true);
        REQUIRE(manager.isStopWord(two) == true);
        REQUIRE(manager.isStopWord(three) == true);
        REQUIRE(manager.isStopWord(nine) == true);
    }
    
    SECTION("中文数字") {
        std::string yi = "一";
        
        REQUIRE(manager.isStopWord(yi) == true);
    }
}

TEST_CASE("停用词 - 英文停用词", "[StopWords][English]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("英文冠词") {
        std::string the_word = "the";
        std::string a_word = "a";
        std::string an_word = "an";
        
        REQUIRE(manager.isStopWord(the_word) == true);
        REQUIRE(manager.isStopWord(a_word) == true);
        REQUIRE(manager.isStopWord(an_word) == true);
    }
    
    SECTION("英文代词") {
        std::string that_word = "that";
        std::string this_word = "this";
        std::string those_word = "those";
        
        REQUIRE(manager.isStopWord(that_word) == true);
        REQUIRE(manager.isStopWord(this_word) == true);
        REQUIRE(manager.isStopWord(those_word) == true);
    }
}

TEST_CASE("停用词 - 边界情况", "[StopWords][Edge]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("空字符串") {
        std::string empty = "";
        REQUIRE(manager.isStopWord(empty) == false);
    }
    
    SECTION("空格") {
        std::string space = " ";
        // 空格通常不在停用词列表中
        // 根据实际停用词文件调整
    }
}

// ==================== 非停用词测试 ====================

TEST_CASE("非停用词 - 常用名词", "[NonStopWords][Noun]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("人物名词") {
        std::string laoshi = "老师";
        std::string xuesheng = "学生";
        std::string yisheng = "医生";
        std::string gongchengshi = "工程师";
        std::string pengyou = "朋友";
        
        REQUIRE(manager.isStopWord(laoshi) == false);
        REQUIRE(manager.isStopWord(xuesheng) == false);
        REQUIRE(manager.isStopWord(yisheng) == false);
        REQUIRE(manager.isStopWord(gongchengshi) == false);
        REQUIRE(manager.isStopWord(pengyou) == false);
    }
    
    SECTION("地点名词") {
        std::string xuexiao = "学校";
        std::string yiyuan = "医院";
        std::string gongyuan = "公园";
        std::string chengshi = "城市";
        std::string guojia = "国家";
        
        REQUIRE(manager.isStopWord(xuexiao) == false);
        REQUIRE(manager.isStopWord(yiyuan) == false);
        REQUIRE(manager.isStopWord(gongyuan) == false);
        REQUIRE(manager.isStopWord(chengshi) == false);
        REQUIRE(manager.isStopWord(guojia) == false);
    }
    
    SECTION("抽象名词") {
        std::string sixiang = "思想";
        std::string wenhua = "文化";
        std::string zhishi = "知识";
        std::string kexue = "科学";
        std::string jishu = "技术";
        
        REQUIRE(manager.isStopWord(sixiang) == false);
        REQUIRE(manager.isStopWord(wenhua) == false);
        REQUIRE(manager.isStopWord(zhishi) == false);
        REQUIRE(manager.isStopWord(kexue) == false);
        REQUIRE(manager.isStopWord(jishu) == false);
    }
}

TEST_CASE("非停用词 - 常用动词", "[NonStopWords][Verb]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("基本动词") {
        std::string xuexi = "学习";
        std::string gongzuo = "工作";
        std::string shenghuo = "生活";
        std::string kaifa = "开发";
        std::string yanjiu = "研究";
        
        REQUIRE(manager.isStopWord(xuexi) == false);
        REQUIRE(manager.isStopWord(gongzuo) == false);
        REQUIRE(manager.isStopWord(shenghuo) == false);
        REQUIRE(manager.isStopWord(kaifa) == false);
        REQUIRE(manager.isStopWord(yanjiu) == false);
    }
    
    SECTION("认知动词") {
        std::string lijie = "理解";
        std::string renshi = "认识";
        std::string sikao = "思考";
        std::string faxian = "发现";
        std::string tansuo = "探索";
        
        REQUIRE(manager.isStopWord(lijie) == false);
        REQUIRE(manager.isStopWord(renshi) == false);
        REQUIRE(manager.isStopWord(sikao) == false);
        REQUIRE(manager.isStopWord(faxian) == false);
        REQUIRE(manager.isStopWord(tansuo) == false);
    }
}

TEST_CASE("非停用词 - 形容词", "[NonStopWords][Adjective]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("性质形容词") {
        std::string meili = "美丽";
        std::string congming = "聪明";
        std::string youxiu = "优秀";
        std::string kuaile = "快乐";
        std::string xingfu = "幸福";
        
        REQUIRE(manager.isStopWord(meili) == false);
        REQUIRE(manager.isStopWord(congming) == false);
        REQUIRE(manager.isStopWord(youxiu) == false);
        REQUIRE(manager.isStopWord(kuaile) == false);
        REQUIRE(manager.isStopWord(xingfu) == false);
    }
    
    SECTION("状态形容词") {
        std::string zhongyao = "重要";
        std::string youqu = "有趣";
        std::string kunnan = "困难";
        std::string rongyi = "容易";
        std::string fuza = "复杂";
        
        REQUIRE(manager.isStopWord(zhongyao) == false);
        REQUIRE(manager.isStopWord(youqu) == false);
        REQUIRE(manager.isStopWord(kunnan) == false);
        REQUIRE(manager.isStopWord(rongyi) == false);
        REQUIRE(manager.isStopWord(fuza) == false);
    }
}

TEST_CASE("非停用词 - 专业术语", "[NonStopWords][Technical]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("计算机术语") {
        std::string jisuanji = "计算机";
        std::string ruanjian = "软件";
        std::string yingjian = "硬件";
        std::string wangluo = "网络";
        std::string shujuku = "数据库";
        std::string suanfa = "算法";
        std::string biancheng = "编程";
        
        REQUIRE(manager.isStopWord(jisuanji) == false);
        REQUIRE(manager.isStopWord(ruanjian) == false);
        REQUIRE(manager.isStopWord(yingjian) == false);
        REQUIRE(manager.isStopWord(wangluo) == false);
        REQUIRE(manager.isStopWord(shujuku) == false);
        REQUIRE(manager.isStopWord(suanfa) == false);
        REQUIRE(manager.isStopWord(biancheng) == false);
    }
    
    SECTION("数据结构相关") {
        std::string shujujiegou = "数据结构";
        std::string shuzu = "数组";
        std::string lianbiao = "链表";
        std::string shuzhuang = "树状";
        std::string tupian = "图片";
        std::string paixu = "排序";
        std::string sousuo = "搜索";
        
        REQUIRE(manager.isStopWord(shujujiegou) == false);
        REQUIRE(manager.isStopWord(shuzu) == false);
        REQUIRE(manager.isStopWord(lianbiao) == false);
        REQUIRE(manager.isStopWord(shuzhuang) == false);
        REQUIRE(manager.isStopWord(tupian) == false);
        REQUIRE(manager.isStopWord(paixu) == false);
        REQUIRE(manager.isStopWord(sousuo) == false);
    }
}

TEST_CASE("非停用词 - 常用短语", "[NonStopWords][Phrase]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("四字成语") {
        std::string yimafengxian = "一马当先";
        std::string qixinxieli = "齐心协力";
        // 注意：成语可能会被分词，这里测试的是完整词
    }
    
    SECTION("双字词组") {
        std::string tianqi = "天气";
        std::string sanbu = "散步";
        std::string pinguo = "苹果";
        std::string xiangjiao = "香蕉";
        std::string kafei = "咖啡";
        
        REQUIRE(manager.isStopWord(tianqi) == false);
        REQUIRE(manager.isStopWord(sanbu) == false);
        REQUIRE(manager.isStopWord(pinguo) == false);
        REQUIRE(manager.isStopWord(xiangjiao) == false);
        REQUIRE(manager.isStopWord(kafei) == false);
    }
}

TEST_CASE("非停用词 - 专有名词", "[NonStopWords][ProperNoun]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("常见专有名词") {
        std::string zhongguo = "中国";
        std::string beijing = "北京";
        std::string shanghai = "上海";
        
        REQUIRE(manager.isStopWord(zhongguo) == false);
        REQUIRE(manager.isStopWord(beijing) == false);
        REQUIRE(manager.isStopWord(shanghai) == false);
    }
}

TEST_CASE("非停用词 - 数值和单位", "[NonStopWords][NumberUnit]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("复合数值") {
        // 这些通常不是停用词
        std::string shige = "十个";
        std::string baifen = "百分";
        
        // 根据实际情况测试
    }
    
    SECTION("单位词") {
        std::string gongjin = "公斤";
        std::string gongli = "公里";
        std::string xiaoshi = "小时";
        
        REQUIRE(manager.isStopWord(gongjin) == false);
        REQUIRE(manager.isStopWord(gongli) == false);
        REQUIRE(manager.isStopWord(xiaoshi) == false);
    }
}

TEST_CASE("非停用词 - 英文实词", "[NonStopWords][EnglishContent]") {
    StopWordsManager manager("../data/dict/stop_words.utf8");
    
    SECTION("英文名词") {
        std::string program = "program";
        std::string algorithm = "algorithm";
        std::string database = "database";
        std::string network = "network";
        
        REQUIRE(manager.isStopWord(program) == false);
        REQUIRE(manager.isStopWord(algorithm) == false);
        REQUIRE(manager.isStopWord(database) == false);
        REQUIRE(manager.isStopWord(network) == false);
    }
    
    SECTION("英文动词") {
        std::string learn = "learn";
        std::string develop = "develop";
        std::string create = "create";
        std::string build = "build";
        
        REQUIRE(manager.isStopWord(learn) == false);
        REQUIRE(manager.isStopWord(develop) == false);
        REQUIRE(manager.isStopWord(create) == false);
        REQUIRE(manager.isStopWord(build) == false);
    }
}


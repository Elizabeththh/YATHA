#include "../third_party/cppjieba/Jieba.hpp"
#include "../third_party/cppjieba/limonp/ArgvContext.hpp"
#include "../include/ha_engine.h"
#include "../third_party/cpp_httplib/httplib.h"
#include <iostream>
#include <sstream>

const std::string DICT_PATH = "dict/jieba.dict.utf8";
const std::string HMM_PATH = "dict/hmm_model.utf8";
const std::string USER_DICT_PATH = "dict/user.dict.utf8";
const std::string IDF_PATH = "dict/idf.utf8";
const std::string STOP_WORD_DICT_PATH = "dict/stop_words.utf8";
// const std::string SENSITIVE_WORD_PATH      =     "dict/sensitive_words.utf8";

void runWebServer();

int main(int argc, char *argv[])
{
    std::string inputFile{};
    std::string outputFile{};

    // 默认参数
    int topK = 3;
    int windowSize = 600;
    std::unordered_set<std::string> filter{};
    std::unordered_set<std::string> chooser{};
    bool isServer = false;

    limonp::ArgvContext arg(argc, argv);
    arg.ReadArgv(inputFile, outputFile, windowSize, topK, filter, chooser, isServer);

    if (isServer)
    {
        runWebServer();
        return 0;
    }

    std::ofstream out(outputFile, std::ios::binary);
    if (!out.is_open())
    {
        std::cerr << "[错误]：无法读取输出文件 " << outputFile << std::endl;
        return EXIT_FAILURE;
    }

    HaEngine ha(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_DICT_PATH, windowSize, topK, filter, chooser, inputFile, outputFile);
    if (filter.size() == 0 && chooser.size() == 0)
        ha.cutWord();
    else if (filter.size() != 0)
        ha.cutWordFilter();
    else if (chooser.size() != 0)
        ha.cutWordChooser();
    else
        ha.cutWord();
    // ha.cutWordsTest();
    // ha.testOutput();
}

// 开启服务器
void runWebServer()
{
    httplib::Server svr;

    svr.set_mount_point("/", "../web");
    svr.set_mount_point("/img", "../img");

    svr.Post("/api/analyze", [](const httplib::Request &req, httplib::Response &res)
    {
        std::string inputContent = req.body;

        std::string tempInput = "temp_input.txt";
        std::string tempOutput = "temp_output.txt";

        {
            std::ofstream ofs(tempInput);
            ofs << inputContent;
        }

        {
            std::unordered_set<std::string> filter;
            std::unordered_set<std::string> chooser;
            HaEngine ha(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_DICT_PATH, 600, 10, filter, chooser, tempInput, tempOutput);
            ha.cutWord();
        } 

        std::ifstream ifs(tempOutput);
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        
        res.set_content(content, "text/plain"); 
    });

    svr.Post("/api/analyze-filter", [](const httplib::Request &req, httplib::Response &res)
    {
        std::string jsonBody = req.body;
        
        // 提取 content 字段，一定要注意处理转义字符！（接受的数据不是text/plain）
        size_t contentStart = jsonBody.find("\"content\":\"");
        if (contentStart == std::string::npos) {
            res.set_content("JSON解析错误", "text/plain");
            return;
        }
        contentStart += 11;                 // "content":"的长度
        
        std::string inputContent;
        for (size_t i = contentStart; i < jsonBody.length(); i++) {
            if (jsonBody[i] == '\\' && i + 1 < jsonBody.length()) 
            {
                if (jsonBody[i + 1] == 'n') { inputContent += '\n'; i++; }
                else if (jsonBody[i + 1] == 't') { inputContent += '\t'; i++; }
                else if (jsonBody[i + 1] == '\"') { inputContent += '\"'; i++; }
                else if (jsonBody[i + 1] == '\\') { inputContent += '\\'; i++; }
                else inputContent += jsonBody[i];
            } 
            else if (jsonBody[i] == '\"' && (i == 0 || jsonBody[i-1] != '\\')) 
                break; // 找到content字段的结束引号
            else 
                inputContent += jsonBody[i];
        }
        
        // 提取 pos 字段
        size_t posStart = jsonBody.find("\"pos\":\"");
        std::string posString;
        if (posStart != std::string::npos) 
        {
            posStart += 7;
            size_t posEnd = jsonBody.find("\"", posStart);
            if (posEnd != std::string::npos) 
                posString = jsonBody.substr(posStart, posEnd - posStart);
        }
        
        // 将 pos 字符串分割成 unordered_set
        std::unordered_set<std::string> filter;
        std::stringstream ss(posString);
        std::string pos;
        while (std::getline(ss, pos, ',')) 
        {
            if (!pos.empty()) 
                filter.insert(pos);
        }
        
        // 保存 content 到临时文件
        std::string tempInput = "temp_filter_input.txt";
        std::string tempOutput = "temp_filter_output.txt";
        {
            std::ofstream ofs(tempInput);
            ofs << inputContent;
        }
        
        // 创建 HaEngine 并执行过滤分析
        {
            std::unordered_set<std::string> chooser;
            HaEngine ha(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_DICT_PATH, 
                       600, 10, filter, chooser, tempInput, tempOutput);
            ha.cutWordFilter();
        }
        
        // 读取结果并返回
        std::ifstream ifs(tempOutput);
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        res.set_content(content, "text/plain"); 
    });

    svr.Post("/api/analyze-chooser", [](const httplib::Request &req, httplib::Response &res)
    {
        std::string jsonBody = req.body;
        
        // 提取 content 字段，一定要注意处理转义字符！（接受的数据不是text/plain）
        size_t contentStart = jsonBody.find("\"content\":\"");
        if (contentStart == std::string::npos) 
        {
            res.set_content("JSON解析错误", "text/plain");
            return;
        }
        contentStart += 11;
        
        std::string inputContent;
        for (size_t i = contentStart; i < jsonBody.length(); i++) 
        {
            if (jsonBody[i] == '\\' && i + 1 < jsonBody.length()) 
            {
                if (jsonBody[i + 1] == 'n') { inputContent += '\n'; i++; }
                else if (jsonBody[i + 1] == 't') { inputContent += '\t'; i++; }
                else if (jsonBody[i + 1] == '\"') { inputContent += '\"'; i++; }
                else if (jsonBody[i + 1] == '\\') { inputContent += '\\'; i++; }
                else inputContent += jsonBody[i];
            } 
            else if (jsonBody[i] == '\"' && (i == 0 || jsonBody[i-1] != '\\'))
                break;
            else 
                inputContent += jsonBody[i];
        }
        
        // 提取 pos 字段
        size_t posStart = jsonBody.find("\"pos\":\"");
        std::string posString;
        if (posStart != std::string::npos) 
        {
            posStart += 7;
            size_t posEnd = jsonBody.find("\"", posStart);
            if (posEnd != std::string::npos) 
                posString = jsonBody.substr(posStart, posEnd - posStart);
        }
        
        // 将 pos 字符串分割成 unordered_set
        std::unordered_set<std::string> chooser;
        std::stringstream ss(posString);
        std::string pos;
        while (std::getline(ss, pos, ',')) 
        {
            if (!pos.empty())
                chooser.insert(pos);
        }
        
        // 保存 content 到临时文件
        std::string tempInput = "temp_chooser_input.txt";
        std::string tempOutput = "temp_chooser_output.txt";
        {
            std::ofstream ofs(tempInput);
            ofs << inputContent;
        }
        
        // 创建 HaEngine 并执行放行分析
        {
            std::unordered_set<std::string> filter;
            HaEngine ha(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_DICT_PATH, 
                       600, 10, filter, chooser, tempInput, tempOutput);
            ha.cutWordChooser();
        }
        
        // 读取结果并返回
        std::ifstream ifs(tempOutput);
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        res.set_content(content, "text/plain"); 
    });

    std::cout << "服务已在 http://localhost:8080 启动" << std::endl;
    svr.listen("0.0.0.0", 8080);
}
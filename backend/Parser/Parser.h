#pragma once

#include <string>
#include <future>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <json.hpp>
#include "thread_pool.h"  

using json = nlohmann::json;

class Parser {
public:
    explicit Parser(thread_pool& pool);
    ~Parser() = default;

    // Запускает обход rootPath, сразу возвращает future
    std::future<json> parse(const std::string& rootPath);

private:
    thread_pool& m_pool;

    // Разделяемые данные между задачами
    struct SharedData {
        json audio = json::array();
        json video = json::array();
        json images = json::array();
        std::mutex dataMutex;          // защита трёх массивов
        std::atomic<int> pending{0};   // счётчик незавершённых задач
        std::mutex cvMutex;
        std::condition_variable cv;
        bool finished = false;
    };

    // Обрабатывает одну директорию: классифицирует файлы,
    // добавляет поддиректории как новые задачи
    void processDirectory(const std::string& dirPath,
                          std::shared_ptr<SharedData> data);
};
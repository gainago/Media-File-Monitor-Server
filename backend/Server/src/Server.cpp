#include "httplib.h"
#include <iostream>
#include <memory>

#include "thread_pool.h"
#include "Parser.h"

std::mutex read_json;
json information;

//этот поток раз в polling_period секунд будет обновлять json 
void polling_thread(std::string start_directory, std::size_t polling_period)
{
    thread_pool pool(10);
    Parser parser(pool); 
    while (true) {
        {
            std::lock_guard<std::mutex> lock(read_json);
            std::future<json> result = parser.parse(start_directory);
            information = result.get();   // блокируется до готовности
        }
    
        std::this_thread::sleep_for(std::chrono::seconds(polling_period));
    }
}

// Вспомогательная функция для чтения файла в строку
static std::string readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

int startServer(std::string start_directory, std::size_t polling_period) {

    // HTTP-сервер 
    httplib::Server svr; // По умолчанию услопльзует множество потоков.

    // Создаем еще один поток, чтобы он вызвал polling_thread
    // и выполнял ее вечно(до конца работы программы).
    std::thread([start_directory, polling_period]() -> void {
        polling_thread(start_directory, polling_period);
        return;
    }).detach();

    //Путь относительно места к которому обращается клиент
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        // Путь относительно build
        std::string content = readFile("../frontend/public/index.html");
        if (content.empty()) {
            res.status = 404;
            res.set_content("index.html not found", "text/plain");
        } else {
            res.set_content(content, "text/html");
        }
    });

    svr.Get("/main.js", [](const httplib::Request&, httplib::Response& res) {
        std::string content = readFile("../frontend/public/main.js");
        if (content.empty()) {
            res.status = 404;
            res.set_content("main.js not found", "text/plain");
        } else {
            res.set_content(content, "application/javascript");
        }
    });

    svr.Get("/style.css", [](const httplib::Request&, httplib::Response& res) {
        std::string content = readFile("../frontend/public/style.css");
        if (content.empty()) {
            res.status = 404;
            res.set_content("style.css not found", "text/plain");
        } else {
            res.set_content(content, "text/css");
        }
    });

    // SSE-поток. Не закрывается после отправки и сервер раз в polling_period отправляет данные
    svr.Get("/media_files/stream", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content_provider(
            "text/event-stream", // Специальная строка, получил которую библиотека начинает работать по протоколу SSE
            [](size_t, httplib::DataSink& sink) {
                while (sink.is_writable()) {
                
                    std::lock_guard<std::mutex> lock(read_json);
                    std::string msg = "data: " + information.dump() + "\n\n";
                    sink.write(msg.data(), msg.size());
                }
                return true;
            },
            [](bool) {} // не вызываем функцию при закрытии соединения
        );
    });

    svr.Get("/media_files", [&](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(read_json);
        res.set_content(information.dump(), "application/json");
});

    std::cout << "Server listening on http://0.0.0.0:1234\n";
    svr.listen("0.0.0.0", 1234);
    std::cout << "Server stop listening\n";

    return 0;
}
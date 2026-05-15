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

int startServer(std::string start_directory, std::size_t polling_period) {

    // HTTP-сервер 
    httplib::Server svr; // По умолчанию услопльзует множество потоков.

    // Создаем еще один поток, чтобы он вызвал polling_thread
    // и выполнял ее вечно(до конца работы программы).
    std::thread([start_directory, polling_period]() -> void {
        polling_thread(start_directory, polling_period);
        return;
    }).detach();


    svr.Get("/media_files", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(read_json);
        res.set_content(information.dump(), "application/json");
    });

    std::cout << "Server listening on http://0.0.0.0:1234\n";
    svr.listen("0.0.0.0", 1234);
    std::cout << "Server stop listening\n";

    return 0;
}
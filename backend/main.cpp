#include "Parser.h"
#include <iostream>

std::mutex output_mutex;

int main(int argc, char *argv[]) {
    thread_pool pool(10);       // создаём пул из 4 потоков
    Parser parser(pool);       // парсер использует этот пул

    std::string directory = "/home/goshagaina/Pictures/saa";  // или любой путь
    std::size_t polling_period = 30; //seconds
    if (argc > 3) {
        directory = argv[1];
        polling_period = std::stoll(argv[1]);
    }

    std::future<json> result = parser.parse(directory);

    std::cout << "Обход запущен асинхронно, ждём...\n";
    json media = result.get();   // блокируется до готовности

    std::cout << media.dump(2) << std::endl;
    return 0;
}
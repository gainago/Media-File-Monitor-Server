#include <filesystem>
#include <algorithm>
#include <set>
#include <unistd.h>

#include "Parser.h"
#include "output_mutex.h"

const std::set<std::string> audio_extension = {
    ".mp3", ".wav", ".flac", ".aac", ".wma"
};
const std::set<std::string> video_extension = {
    ".mpg", ".mpeg", ".mp4",
};
const std::set<std::string> image_extension = {
    ".jpg", ".jpeg", ".png", ".gif", ".bmp"
};

Parser::Parser(thread_pool& pool) : m_pool(pool) {}

std::future<json> Parser::parse(const std::string& root_path)
{
    auto promise = std::make_shared<std::promise<json>>();
    std::future<json> future = promise->get_future();

    auto data = std::make_shared<SharedData>();

    data->pending = 1;
    m_pool.push_task([this, root_path, data]() {
        processDirectory(root_path, data);
    });

    // Запускаем фоновый поток, который дождётся завершения обхода
    // и установит promise; data и promise  копируются как shared_ptr
    std::thread([promise, data]() {
        std::unique_lock<std::mutex> lock(data->cvMutex);
        data->cv.wait(lock, [&data] { return data->finished; });

        json result;
        result["audio"] = std::move(data->audio);
        result["video"] = std::move(data->video);
        result["images"] = std::move(data->images);
        promise->set_value(std::move(result));
    }).detach();

    return future;
}

void Parser::processDirectory(const std::string &dir_path,
                              std::shared_ptr<SharedData> data) {

    
                
    if (std::filesystem::exists(dir_path) && std::filesystem::is_directory(dir_path)) {

        std::error_code code;
        std::filesystem::directory_iterator it = std::filesystem::directory_iterator(dir_path, code);
        if (code) {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cerr << "Could not get access to " << dir_path.c_str() << '\n';
        
        } else {
            for (const auto &entry : std::filesystem::directory_iterator(dir_path)) {

                if (entry.is_symlink()) {
                    continue;
                }

                std::filesystem::path path = entry.path();
                //std::filesystem::file_status status = std::filesystem::status(entry);
                //std::filesystem::perms perms = status.permissions();
                if (::access(path.c_str(), R_OK) == -1) {
                    std::lock_guard<std::mutex> lock(output_mutex);
                    std::cerr << "Could not get access to " << path.c_str() << '\n';
                    continue;
                }

                if (entry.is_regular_file()) {

                    std::string extension = entry.path().extension().string();
                    //std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    std::string name = entry.path().filename().string();

                    std::lock_guard<std::mutex> lock(data->dataMutex);
                    if (audio_extension.count(extension))
                        data->audio.push_back(name);
                    else if (video_extension.count(extension))
                        data->video.push_back(name);
                    else if (image_extension.count(extension))
                        data->images.push_back(name);
                }
                else if (entry.is_directory()) {
                    // Увеличиваем счетчик задач
                    data->pending++;
                    std::string subdir = entry.path().string();
                    // Захватываем data по shared_ptr в лямбду
                    m_pool.push_task([this, subdir, data]() {
                        processDirectory(subdir, data);
                    });
                }
            }
        }
    }

    --data->pending;
    if (data->pending == 0) {
        // Все задачи выполнены – оповещаем ожидающий поток
        std::lock_guard<std::mutex> lock(data->cvMutex);
        data->finished = true;
        data->cv.notify_all();
    }
}
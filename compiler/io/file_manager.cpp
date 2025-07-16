#include "file_manager.hpp"
#include <filesystem>
#include <fstream>

namespace cmp {

std::vector<std::string> get_all_files_in_folder(const std::string& folder_path) {
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

std::string get_text_from_file(const std::string& file_path) {
    std::ifstream file(file_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

}
#pragma once
#include <string>
#include <vector>

namespace cmp {

std::vector<std::string> get_all_files_in_folder(const std::string& folder_path);
std::string get_text_from_file(const std::string& file_path);

}
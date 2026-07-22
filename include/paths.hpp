#pragma once
#include <filesystem>

namespace paths {
    std::filesystem::path executable_dir();
    std::filesystem::path asset(const std::string& relative_path);
}
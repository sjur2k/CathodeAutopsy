#include "paths.hpp"
#include <windows.h>

namespace paths {

std::filesystem::path executable_dir(){
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
}

std::filesystem::path asset(const std::string& relative_path){
    return executable_dir() / relative_path;
}

}
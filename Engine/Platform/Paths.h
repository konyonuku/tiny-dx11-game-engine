#pragma once
#include <filesystem>

namespace Platform
{
    // 실행파일 절대경로용
    std::filesystem::path GetExecutableDirectory();
}

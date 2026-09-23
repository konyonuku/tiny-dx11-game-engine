#include "Platform/Paths.h"

#include <windows.h>

#include <string>

#include "Core/Log.h"

namespace Platform
{
    std::filesystem::path GetExecutableDirectory()
    {
        std::wstring buffer(MAX_PATH, L'\0');
        for (;;)
        {
            const DWORD capacity = static_cast<DWORD>(buffer.size());
            const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), capacity);
            if (length == 0) {
                Core::LogError("GetModuleFileNameW failed. err=%lu", GetLastError());
                return {};
            }
            
            if (length < capacity) {
                buffer.resize(length);
                return std::filesystem::path(buffer).parent_path();
            }
            if (buffer.size() >= 32768) {
                Core::LogError("Executable path is too long.");
                return {};
            }
            buffer.resize(buffer.size() * 2);
        }
    }
}

#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "Shell.h"

std::string getUsername() {
    const char* user = std::getenv("USER");

#ifdef _WIN32
    if (user == nullptr) {
        user = std::getenv("USERNAME");
    }
#endif

    return user == nullptr ? "user" : std::string(user);
}

std::string getHostname() {
    char hostname[256]{};

#ifdef _WIN32
    DWORD size = sizeof(hostname);
    if (GetComputerNameA(hostname, &size)) {
        return hostname;
    }
#else
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return hostname;
    }
#endif

    return "computer";
}

int main(int argc, char* argv[]) {
    std::string vfsPath;
    std::string scriptPath;

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];

        if (argument == "--vfs" && i + 1 < argc) {
            vfsPath = argv[++i];
        } else if (argument == "--script" && i + 1 < argc) {
            scriptPath = argv[++i];
        } else {
            std::cerr << "Ошибка: неизвестный или неполный параметр: "
                      << argument << '\n';
            return 1;
        }
    }

    std::cout << "Конфигурация эмулятора:\n";
    std::cout << "VFS: "
              << (vfsPath.empty() ? "не задана" : vfsPath) << '\n';
    std::cout << "Стартовый скрипт: "
              << (scriptPath.empty() ? "не задан" : scriptPath) << "\n\n";

    Shell shell(getUsername(), getHostname());

    if (!scriptPath.empty() && !shell.runScript(scriptPath)) {
        return 1;
    }

    shell.run();
    return 0;
}

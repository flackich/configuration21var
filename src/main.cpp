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

int main() {
    Shell shell(getUsername(), getHostname());
    shell.run();
    return 0;
}

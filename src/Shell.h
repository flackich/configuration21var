#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "VirtualFileSystem.h"

class Shell {
public:
    Shell(std::string username, std::string hostname);

    bool loadVfs(const std::string& path);
    bool runScript(const std::string& path);
    void run();

private:
    enum class CommandResult {
        Success,
        Error,
        Exit
    };

    std::string username_;
    std::string hostname_;
    VirtualFileSystem vfs_;
    std::chrono::steady_clock::time_point startedAt_;

    std::vector<std::string> parseCommand(const std::string& input) const;
    CommandResult execute(const std::vector<std::string>& args);
    std::string prompt() const;
};
#pragma once

#include <string>
#include <vector>

class Shell {
public:
    Shell(std::string username, std::string hostname);
    bool runScript(const std::string& path);
    void run();

private:
    std::string username_;
    std::string hostname_;

    std::vector<std::string> parseCommand(const std::string& input) const;
    bool execute(const std::vector<std::string>& args, bool& errorOccurred);
    std::string prompt() const;
};

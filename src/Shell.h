#pragma once

#include <string>
#include <vector>

class Shell {
public:
    Shell(std::string username, std::string hostname);
    void run();

private:
    std::string username_;
    std::string hostname_;

    std::vector<std::string> parseCommand(const std::string& input) const;
    bool execute(const std::vector<std::string>& args);
    std::string prompt() const;
};

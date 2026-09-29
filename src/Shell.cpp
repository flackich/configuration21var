#include "Shell.h"

#include <iostream>
#include <stdexcept>
#include <utility>

Shell::Shell(std::string username, std::string hostname)
    : username_(std::move(username)),
      hostname_(std::move(hostname)) {}

std::vector<std::string> Shell::parseCommand(const std::string& input) const {
    std::vector<std::string> result;
    std::string current;
    bool inQuotes = false;

    for (char symbol : input) {
        if (symbol == '"') {
            inQuotes = !inQuotes;
        } else if ((symbol == ' ' || symbol == '\t') && !inQuotes) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += symbol;
        }
    }

    if (inQuotes) {
        throw std::invalid_argument("незакрытые кавычки");
    }

    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}

std::string Shell::prompt() const {
    return username_ + "@" + hostname_ + ":~$ ";
}

bool Shell::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return true;
    }

    const std::string& command = args[0];

    if (command == "exit") {
        if (args.size() != 1) {
            std::cerr << "Ошибка: exit не принимает аргументы\n";
            return true;
        }
        return false;
    }

    if (command == "ls") {
        if (args.size() > 2) {
            std::cerr << "Ошибка: ls принимает не более одного аргумента\n";
        } else {
            std::cout << "ls: команда-заглушка\n";
        }
        return true;
    }

    if (command == "cd") {
        if (args.size() != 2) {
            std::cerr << "Ошибка: cd требует один аргумент\n";
        } else {
            std::cout << "cd: команда-заглушка\n";
        }
        return true;
    }

    std::cerr << "Ошибка: неизвестная команда: " << command << '\n';
    return true;
}

void Shell::run() {
    std::string input;

    while (true) {
        std::cout << prompt();

        if (!std::getline(std::cin, input)) {
            std::cout << '\n';
            break;
        }

        try {
            const auto args = parseCommand(input);
            if (!execute(args)) {
                break;
            }
        } catch (const std::exception& error) {
            std::cerr << "Ошибка: " << error.what() << '\n';
        }
    }
}

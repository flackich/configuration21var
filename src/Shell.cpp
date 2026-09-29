#include "Shell.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>

Shell::Shell(std::string username, std::string hostname)
    : username_(std::move(username)),
      hostname_(std::move(hostname)) {}

bool Shell::loadVfs(const std::string& path) {
    std::string error;

    if (!vfs_.load(path, error)) {
        std::cerr << "Ошибка загрузки VFS: " << error << '\n';
        return false;
    }

    return true;
}

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

bool Shell::execute(const std::vector<std::string>& args,
                    bool& errorOccurred) {
    errorOccurred = false;

    if (args.empty()) {
        return true;
    }

    const std::string& command = args[0];
    std::string error;

    if (command == "exit") {
        if (args.size() != 1) {
            std::cerr << "Ошибка: exit не принимает аргументы\n";
            errorOccurred = true;
            return true;
        }
        return false;
    }

    if (command == "ls") {
        if (args.size() > 2) {
            std::cerr << "Ошибка: ls принимает не более одного аргумента\n";
            errorOccurred = true;
        } else {
            std::cout << "ls: команда-заглушка\n";
        }
        return true;
    }

    if (command == "cd") {
        if (args.size() != 2) {
            std::cerr << "Ошибка: cd требует один аргумент\n";
            errorOccurred = true;
        } else {
            std::cout << "cd: команда-заглушка\n";
        }
        return true;
    }

    if (command == "vfs-save") {
        if (args.size() != 2) {
            std::cerr << "Ошибка: vfs-save требует путь\n";
            errorOccurred = true;
        } else if (!vfs_.save(args[1], error)) {
            std::cerr << "Ошибка сохранения VFS: " << error << '\n';
            errorOccurred = true;
        } else {
            std::cout << "VFS сохранена: " << args[1] << '\n';
        }
        return true;
    }

    std::cerr << "Ошибка: неизвестная команда: " << command << '\n';
    errorOccurred = true;
    return true;
}

bool Shell::runScript(const std::string& path) {
    std::ifstream file(path);

    if (!file) {
        std::cerr << "Ошибка: не удалось открыть стартовый скрипт: "
                  << path << '\n';
        return false;
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::cout << prompt() << line << '\n';

        try {
            const auto args = parseCommand(line);
            bool errorOccurred = false;
            const bool keepRunning = execute(args, errorOccurred);

            if (errorOccurred) {
                std::cerr
                    << "Ошибка: выполнение стартового скрипта остановлено\n";
                return false;
            }

            if (!keepRunning) {
                return true;
            }
        } catch (const std::exception& error) {
            std::cerr << "Ошибка скрипта: " << error.what() << '\n';
            return false;
        }
    }

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
            bool errorOccurred = false;

            if (!execute(args, errorOccurred)) {
                break;
            }
        } catch (const std::exception& error) {
            std::cerr << "Ошибка: " << error.what() << '\n';
        }
    }
}

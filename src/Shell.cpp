#include "Shell.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Shell::Shell(std::string username, std::string hostname)
    : username_(std::move(username)),
      hostname_(std::move(hostname)),
      startedAt_(std::chrono::steady_clock::now()) {}

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
    const std::string path = vfs_.currentPath();
    std::string shown = "~";
    if (path != "/") {
        shown += path;
    }
    return username_ + "@" + hostname_ + ":" + shown + "$ ";
}

void Shell::printArguments(const std::vector<std::string>& args) const {
    std::cout << "Аргументы:";
    if (args.size() <= 1) {
        std::cout << " отсутствуют";
    } else {
        for (std::size_t i = 1; i < args.size(); ++i) {
            std::cout << " [" << args[i] << "]";
        }
    }
    std::cout << '\n';
}

bool Shell::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return true;
    }

    const std::string& command = args[0];
    std::string error;

    if (command == "exit") {
        if (args.size() != 1) {
            std::cerr << "Ошибка: exit не принимает аргументы\n";
            return true;
        }
        return false;
    }

    if (command == "whoami") {
        if (args.size() != 1) {
            std::cerr << "Ошибка: whoami не принимает аргументы\n";
        } else {
            std::cout << username_ << '\n';
        }
        return true;
    }

    if (command == "uptime") {
        const auto now = std::chrono::steady_clock::now();
        const auto seconds =
            std::chrono::duration_cast<std::chrono::seconds>(now - startedAt_).count();
        std::cout << "Эмулятор работает " << seconds << " сек.\n";
        return true;
    }

    if (command == "ls") {
        if (args.size() > 2) {
            std::cerr << "Ошибка: ls принимает не более одного пути\n";
            return true;
        }
        const auto entries =
            vfs_.list(args.size() == 2 ? args[1] : ".", error);
        if (!error.empty()) {
            std::cerr << "Ошибка: " << error << '\n';
        } else {
            for (const auto& entry : entries) {
                std::cout << entry << '\n';
            }
        }
        return true;
    }

    if (command == "cd") {
        if (args.size() != 2) {
            std::cerr << "Ошибка: cd требует один путь\n";
        } else if (!vfs_.changeDirectory(args[1], error)) {
            std::cerr << "Ошибка: " << error << '\n';
        }
        return true;
    }

    if (command == "wc") {
        if (args.size() != 2) {
            std::cerr << "Ошибка: wc требует имя файла\n";
            return true;
        }

        std::string content;
        if (!vfs_.readFile(args[1], content, error)) {
            std::cerr << "Ошибка: " << error << '\n';
            return true;
        }

        std::istringstream stream(content);
        std::size_t words = 0;
        std::string word;
        while (stream >> word) {
            ++words;
        }

        std::size_t lines = 0;
        for (char symbol : content) {
            if (symbol == '\n') {
                ++lines;
            }
        }
        if (!content.empty() && content.back() != '\n') {
            ++lines;
        }

        std::cout << lines << ' ' << words << ' '
                  << content.size() << ' ' << args[1] << '\n';
        return true;
    }

    if (command == "mkdir") {
        if (args.size() != 2) {
            std::cerr << "Ошибка: mkdir требует имя каталога\n";
        } else if (!vfs_.makeDirectory(args[1], error)) {
            std::cerr << "Ошибка: " << error << '\n';
        }
        return true;
    }

    if (command == "chown") {
        if (args.size() != 3) {
            std::cerr << "Ошибка: chown требует владельца и путь\n";
        } else if (!vfs_.changeOwner(args[1], args[2], error)) {
            std::cerr << "Ошибка: " << error << '\n';
        }
        return true;
    }

    if (command == "vfs-save") {
        if (args.size() != 2) {
            std::cerr << "Ошибка: vfs-save требует путь\n";
        } else if (!vfs_.save(args[1], error)) {
            std::cerr << "Ошибка сохранения VFS: " << error << '\n';
        } else {
            std::cout << "VFS сохранена: " << args[1] << '\n';
        }
        return true;
    }

    std::cerr << "Ошибка: неизвестная команда: " << command << '\n';
    return true;
}

bool Shell::runScript(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Ошибка: не удалось открыть стартовый скрипт\n";
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
            if (!execute(args)) {
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
            if (!execute(args)) {
                break;
            }
        } catch (const std::exception& error) {
            std::cerr << "Ошибка: " << error.what() << '\n';
        }
    }
}

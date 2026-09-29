#include "VirtualFileSystem.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace {

std::string escapeJson(const std::string& value) {
    std::string result;
    for (char c : value) {
        if (c == '\\' || c == '"') {
            result += '\\';
        }
        if (c == '\n') {
            result += "\\n";
        } else {
            result += c;
        }
    }
    return result;
}

void writeNode(std::ostream& out, const VfsNode& node, int indent) {
    const std::string pad(indent, ' ');
    const std::string next(indent + 2, ' ');

    out << "{\n";
    out << next << "\"type\": \""
        << (node.directory ? "directory" : "file") << "\",\n";
    out << next << "\"owner\": \"" << escapeJson(node.owner) << "\"";

    if (node.directory) {
        out << ",\n" << next << "\"children\": {";
        if (!node.children.empty()) {
            out << '\n';
        }

        std::size_t index = 0;
        for (const auto& [name, child] : node.children) {
            out << std::string(indent + 4, ' ') << '"'
                << escapeJson(name) << "\": ";
            writeNode(out, child, indent + 4);
            if (++index != node.children.size()) {
                out << ',';
            }
            out << '\n';
        }

        out << next << "}\n";
    } else {
        out << ",\n" << next << "\"content\": \""
            << escapeJson(node.content) << "\"\n";
    }

    out << pad << '}';
}

class JsonReader {
public:
    explicit JsonReader(std::string text) : text_(std::move(text)) {}

    bool readRoot(VfsNode& node) {
        skip();
        return readNode(node) && finish();
    }

private:
    std::string text_;
    std::size_t pos_ = 0;

    void skip() {
        while (pos_ < text_.size() &&
               std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    bool finish() {
        skip();
        return pos_ == text_.size();
    }

    bool take(char expected) {
        skip();
        if (pos_ >= text_.size() || text_[pos_] != expected) {
            return false;
        }
        ++pos_;
        return true;
    }

    bool readString(std::string& value) {
        skip();
        if (!take('"')) {
            return false;
        }

        value.clear();
        while (pos_ < text_.size()) {
            char c = text_[pos_++];
            if (c == '"') {
                return true;
            }
            if (c == '\\' && pos_ < text_.size()) {
                const char escaped = text_[pos_++];
                value += escaped == 'n' ? '\n' : escaped;
            } else {
                value += c;
            }
        }
        return false;
    }

    bool readChildren(std::map<std::string, VfsNode>& children) {
        if (!take('{')) {
            return false;
        }

        skip();
        if (take('}')) {
            return true;
        }

        while (true) {
            std::string name;
            VfsNode child;
            if (!readString(name) || !take(':') || !readNode(child)) {
                return false;
            }
            children[name] = child;

            skip();
            if (take('}')) {
                return true;
            }
            if (!take(',')) {
                return false;
            }
        }
    }

    bool readNode(VfsNode& node) {
        if (!take('{')) {
            return false;
        }

        bool hasType = false;
        while (true) {
            std::string key;
            if (!readString(key) || !take(':')) {
                return false;
            }

            if (key == "type") {
                std::string type;
                if (!readString(type)) {
                    return false;
                }
                node.directory = type == "directory";
                hasType = type == "directory" || type == "file";
            } else if (key == "owner") {
                if (!readString(node.owner)) {
                    return false;
                }
            } else if (key == "content") {
                if (!readString(node.content)) {
                    return false;
                }
            } else if (key == "children") {
                if (!readChildren(node.children)) {
                    return false;
                }
            } else {
                return false;
            }

            skip();
            if (take('}')) {
                return hasType;
            }
            if (!take(',')) {
                return false;
            }
        }
    }
};

}  // namespace

VirtualFileSystem::VirtualFileSystem() {
    root_.directory = true;
    root_.owner = "root";

    VfsNode documents;
    documents.directory = true;

    VfsNode study;
    study.directory = true;

    VfsNode info;
    info.directory = false;
    info.content = "Configuration management\nVariant 21 emulator\n";

    study.children["info.txt"] = info;
    documents.children["study"] = study;
    root_.children["Documents"] = documents;
}

std::vector<std::string>
VirtualFileSystem::normalize(const std::string& path) const {
    std::vector<std::string> result = path.empty() || path[0] == '/' ?
        std::vector<std::string>{} : cwd_;

    std::stringstream stream(path);
    std::string part;

    while (std::getline(stream, part, '/')) {
        if (part.empty() || part == ".") {
            continue;
        }
        if (part == "..") {
            if (!result.empty()) {
                result.pop_back();
            }
        } else {
            result.push_back(part);
        }
    }
    return result;
}

const VfsNode*
VirtualFileSystem::findNode(const std::vector<std::string>& parts) const {
    const VfsNode* node = &root_;
    for (const auto& part : parts) {
        const auto it = node->children.find(part);
        if (!node->directory || it == node->children.end()) {
            return nullptr;
        }
        node = &it->second;
    }
    return node;
}

VfsNode* VirtualFileSystem::findNode(const std::vector<std::string>& parts) {
    return const_cast<VfsNode*>(
        static_cast<const VirtualFileSystem*>(this)->findNode(parts));
}

std::vector<std::string>
VirtualFileSystem::list(const std::string& path, std::string& error) const {
    error.clear();
    const VfsNode* node = findNode(normalize(path));

    if (node == nullptr) {
        error = "путь не найден";
        return {};
    }
    if (!node->directory) {
        return {path};
    }

    std::vector<std::string> result;
    for (const auto& [name, child] : node->children) {
        result.push_back(name + (child.directory ? "/" : ""));
    }
    return result;
}

bool VirtualFileSystem::changeDirectory(const std::string& path,
                                        std::string& error) {
    const auto target = normalize(path);
    const VfsNode* node = findNode(target);

    if (node == nullptr || !node->directory) {
        error = "каталог не найден";
        return false;
    }

    cwd_ = target;
    error.clear();
    return true;
}

bool VirtualFileSystem::makeDirectory(const std::string& path,
                                      std::string& error) {
    auto target = normalize(path);
    if (target.empty()) {
        error = "нельзя создать корневой каталог";
        return false;
    }

    const std::string name = target.back();
    target.pop_back();
    VfsNode* parent = findNode(target);

    if (parent == nullptr || !parent->directory) {
        error = "родительский каталог не найден";
        return false;
    }
    if (parent->children.count(name) != 0) {
        error = "объект уже существует";
        return false;
    }

    parent->children[name] = VfsNode{};
    error.clear();
    return true;
}

bool VirtualFileSystem::changeOwner(const std::string& owner,
                                    const std::string& path,
                                    std::string& error) {
    VfsNode* node = findNode(normalize(path));
    if (node == nullptr) {
        error = "объект не найден";
        return false;
    }

    node->owner = owner;
    error.clear();
    return true;
}

bool VirtualFileSystem::readFile(const std::string& path,
                                 std::string& content,
                                 std::string& error) const {
    const VfsNode* node = findNode(normalize(path));
    if (node == nullptr || node->directory) {
        error = "файл не найден";
        return false;
    }

    content = node->content;
    error.clear();
    return true;
}

std::string VirtualFileSystem::currentPath() const {
    if (cwd_.empty()) {
        return "/";
    }

    std::string result;
    for (const auto& part : cwd_) {
        result += "/" + part;
    }
    return result;
}

bool VirtualFileSystem::load(const std::string& path, std::string& error) {
    std::ifstream file(path);
    if (!file) {
        error = "файл не найден: " + path;
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    VfsNode loaded;
    JsonReader reader(buffer.str());
    if (!reader.readRoot(loaded) || !loaded.directory) {
        error = "некорректный формат JSON VFS";
        return false;
    }

    root_ = loaded;
    cwd_.clear();
    error.clear();
    return true;
}

bool VirtualFileSystem::save(const std::string& path,
                             std::string& error) const {
    std::ofstream file(path);
    if (!file) {
        error = "не удалось открыть файл для записи";
        return false;
    }

    writeNode(file, root_, 0);
    file << '\n';
    error.clear();
    return true;
}

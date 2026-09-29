#pragma once

#include <map>
#include <string>
#include <vector>

struct VfsNode {
    bool directory = true;
    std::string content;
    std::string owner = "root";
    std::map<std::string, VfsNode> children;
};

class VirtualFileSystem {
public:
    VirtualFileSystem();

    bool load(const std::string& path, std::string& error);
    bool save(const std::string& path, std::string& error) const;

    std::vector<std::string> list(const std::string& path, std::string& error) const;
    bool changeDirectory(const std::string& path, std::string& error);
    bool makeDirectory(const std::string& path, std::string& error);
    bool changeOwner(const std::string& owner, const std::string& path,
                     std::string& error);
    bool readFile(const std::string& path, std::string& content,
                  std::string& error) const;

    std::string currentPath() const;

private:
    VfsNode root_;
    std::vector<std::string> cwd_;

    std::vector<std::string> normalize(const std::string& path) const;
    const VfsNode* findNode(const std::vector<std::string>& parts) const;
    VfsNode* findNode(const std::vector<std::string>& parts);
};

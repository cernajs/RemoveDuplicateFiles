#ifndef FILE_FILTER_POLICY_HPP
#define FILE_FILTER_POLICY_HPP

#include <filesystem>

struct FileFilterPolicy {
    virtual bool shouldProcessFile(const std::filesystem::path& line) = 0;
    virtual ~FileFilterPolicy() = default;
};

#endif
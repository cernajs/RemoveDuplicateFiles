#ifndef FILE_FILTER_POLICIES_HPP
#define FILE_FILTER_POLICIES_HPP

#include "file_filter_policy.hpp"
#include <string>

struct NameContainsFilterPolicy : FileFilterPolicy {
    std::string nameContains;

    NameContainsFilterPolicy(const std::string& name) : nameContains(name) {}

    bool shouldProcessFile(const std::filesystem::path& filePath) override {
        return filePath.filename().string().find(nameContains) != std::string::npos;
    }
};

struct ExtensionFilterPolicy : FileFilterPolicy {
    std::string extension;

    ExtensionFilterPolicy(const std::string& ext) : extension(ext) {}

    bool shouldProcessFile(const std::filesystem::path& filePath) override {
        return filePath.extension() == extension;
    }
};

struct NoHiddenFilesFilterPolicy : FileFilterPolicy {
    NoHiddenFilesFilterPolicy() = default;

    bool shouldProcessFile(const std::filesystem::path& filePath) override {
        return filePath.filename().string().front() != '.';
    }
};

struct CompositeFilterPolicy : FileFilterPolicy {
    std::vector<std::unique_ptr<FileFilterPolicy>> filters;

    CompositeFilterPolicy() = default;

    CompositeFilterPolicy(const CompositeFilterPolicy&) = delete;
    CompositeFilterPolicy& operator=(const CompositeFilterPolicy&) = delete;

    CompositeFilterPolicy(CompositeFilterPolicy&&) noexcept = default;
    CompositeFilterPolicy& operator=(CompositeFilterPolicy&&) noexcept = default;

    void addFilter(std::unique_ptr<FileFilterPolicy> filter) {
        filters.emplace_back(std::move(filter));
    }

    bool shouldProcessFile(const std::filesystem::path& filePath) override {
        for (const auto& filter : filters) {
            if (!filter->shouldProcessFile(filePath)) {
                return false;
            }
        }
        return true;
    }
};

#endif
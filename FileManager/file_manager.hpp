#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#define PARALLEL

#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include "../SeparationPolicy/separation_policies.hpp"
#include "../FileFilterPolicy/file_filter_policies.hpp"
#include "../DomainManager/domain_manager.hpp"

template<typename T, template<typename> class SeparationPolicy, class FileFilterPolicy>
class FileManager {
  FileFilterPolicy _filter;
  std::string _path;

  std::unique_ptr<DomainManager> _domainManager;

  void processFile(const std::filesystem::path& filePath) {
    FileProcessor<T, SeparationPolicy> fileProcessor;

    fileProcessor.setFileName(filePath.filename().string());

    std::ifstream file(filePath.string());
    std::string line;
    while (std::getline(file, line)) {
        fileProcessor.parseLine(line);
    }

    fileProcessor.finalizeCurrentDomain();

    const auto& domains = fileProcessor.getDomains();
    _domainManager->addDomains(filePath.filename().string(), domains);
  }

public:
  FileManager(const std::string& path, FileFilterPolicy filter) : _path(path), _filter(std::move(filter)) {
    _domainManager = std::make_unique<DomainManager>();
  } 

  void processFileSubset(const std::vector<std::filesystem::path>& fileSubset) {
      for (const auto& filePath : fileSubset) {
          processFile(filePath);
      }
    }
#ifdef PARALLEL
  void parseDomains() {
    std::vector<std::filesystem::path> files;

    for (const auto& dir : std::filesystem::recursive_directory_iterator(_path)) {
        if (std::filesystem::is_regular_file(dir) && _filter.shouldProcessFile(dir.path().filename())) {
            files.push_back(dir.path());
        }
    }

    size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    size_t filesPerThread = files.size() / numThreads;

    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * filesPerThread;
        size_t end = (i + 1) * filesPerThread;
        if (i == numThreads - 1) end = files.size();

        std::vector<std::filesystem::path> fileSubset(files.begin() + start, files.begin() + end);
        threads.emplace_back(&FileManager::processFileSubset, this, fileSubset);
    }

    for (auto& thread : threads) {
        thread.join();
    }
  }
#else
  void parseDomains() {
    for(const auto& dir : std::filesystem::recursive_directory_iterator(_path)) {
      if(std::filesystem::is_regular_file(dir) && _filter.shouldProcessFile(dir.path().filename())) {
        processFile(dir.path());
      }
    }
  }
#endif

  const DomainManager* getDomainManager() const noexcept { return _domainManager.get(); }
};

#endif // FILE_MANAGER_HPP
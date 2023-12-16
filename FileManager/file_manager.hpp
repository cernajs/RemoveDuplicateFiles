#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <filesystem>
#include <fstream>
#include <string>

#include "../SeparationPolicy/separation_policies.hpp"
#include "../FileFilterPolicy/file_filter_policies.hpp"
#include "../DomainManager/domain_manager.hpp"

template<typename T, template<typename> class SeparationPolicy, class FileFilterPolicy>
class FileManager {
  FileProcessor<T, SeparationPolicy> _file;
  FileFilterPolicy _filter;
  std::string _path;

  DomainManager _domainManager;

public:
  FileManager(const std::string& path, FileFilterPolicy filter) : _path(path), _filter(std::move(filter)) {
    _file = FileProcessor<T, SeparationPolicy>();
    _domainManager = DomainManager();
  } 

  void parseDomains() {
    for(const auto& dir : std::filesystem::recursive_directory_iterator(_path)) {
      if(std::filesystem::is_regular_file(dir) && _filter.shouldProcessFile(dir.path().filename())) {
        _file.clearDomains();
        _file.setFileName(dir.path().filename().string());
        std::ifstream file(dir.path().string());
        std::string line;

        while(std::getline(file, line)) {
          _file.parseLine(line);
        }
        _file.finalizeCurrentDomain();

        auto domains = _file.getDomains();

        _domainManager.addDomains(dir.path().filename().string(), domains);
      }
    }
  }

  const DomainManager& getDomainManager() const noexcept { return _domainManager; }
};

#endif // FILE_MANAGER_HPP
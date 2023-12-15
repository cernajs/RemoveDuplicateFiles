#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include "unordered_map"
#include "set"
#include "fstream"

#include "filesystem"

template<typename T>
struct SeparationPolicy {
  virtual bool splitContent(const T& line) = 0;
  virtual ~SeparationPolicy() = default;
};

template<typename T>
struct BlankLineSeparationPolicy : SeparationPolicy<T> {
  bool splitContent(const T& line) override {
    return line.empty();
  }
};

struct Domain {
    std::string _content;
    size_t _hash;

    Domain(const std::string& content) 
        : _content(content), _hash(std::hash<std::string>{}(content)) {}

    bool operator<(const Domain& other) const {
        return _hash < other._hash;
    }
};


template<typename T, template<typename> class SeparationPolicy>
class FileProcessor {
  std::string _file_name;
  SeparationPolicy<T> _policy;
  std::vector<Domain> _domains;
  Domain _current_domain;
public:
  FileProcessor(const std::string& name) : _file_name(name) {
    _current_domain = Domain();
    _policy = SeparationPolicy<T>();
  }

  FileProcessor() = default;

  void setFileName(const std::string& new_name) noexcept { _file_name = new_name; _current_domain = Domain(); }

  void parseLine(const T& line) {
    if(_policy.splitContent(line)) {
      if (!_current_domain._content.empty()) {
          _domains.emplace_back(_current_domain);
          _current_domain = Domain();
        }
    }
    else {
      _current_domain._content += line;
    }
  }

  void finalizeCurrentDomain() {
    if (!_current_domain._content.empty()) {
        _domains.emplace_back(_current_domain);
        _current_domain = Domain();
    }
  }

  void clearDomains() noexcept { _domains.clear(); }

  std::vector<Domain> getDomains() const noexcept { return _domains; }

  std::string getFileName() const noexcept { return _file_name; }


};

struct FileFilterPolicy {
  virtual bool shouldProcessFile(const std::filesystem::path& line) = 0;
  virtual ~FileFilterPolicy() = default;
};

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

template<typename T, template<typename> class SeparationPolicy, class FileFilterPolicy>
class FileManager {
  FileProcessor<T, SeparationPolicy> _file;
  FileFilterPolicy _filter;
  std::string _path;

  std::unordered_map<std::string, std::vector<Domain>> _domains;
  std::set<Domain> _unique_domains;
public:
  FileManager(const std::string& path) : _path(path) {
    _file = FileProcessor<T, SeparationPolicy>();
    _filter = FileFilterPolicy();
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

        _domains[dir.path().filename().string()] = domains;
        for(const auto& domain : domains) {
          _unique_domains.insert(domain);
        }
      }
    }
  }

  void printDomains() {
    for(const auto& [file_name, domains] : _domains) {
      std::cout << file_name << " : " << std::endl;
      for(const auto& domain : domains) {
        std::cout << domain._content << std::endl;
      }
    }
  }

  void printUniqueDomains() {
    for(const auto& domain : _unique_domains) {
      std::cout << domain._content << "   " << std::endl;
    }
  }
};

int main(int argc, char* argv[]) {
  std::vector<std::string> args(argv, argv + argc);
  if(args.size() != 2) {
    std::cerr << "2 args required" << std::endl;
    return 1;
  }

  const std::string& path = args[1];
  FileManager<std::string, BlankLineSeparationPolicy, NoHiddenFilesFilterPolicy> manager(path);
  manager.parseDomains();

  manager.printDomains();
  return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <set>
#include <memory>

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
    std::size_t hash;
    std::string filename;

    Domain() : hash(0) {}

    void addLine(const std::string& line) {
        std::size_t lineHash = std::hash<std::string>{}(line);
        hash ^= lineHash;
        //std::cout << lineHash << std::endl;
    }

    bool operator==(const Domain& other) const {
        return hash == other.hash;
    }

    bool operator<(const Domain& other) const {
        return hash < other.hash;
    }
};



template<typename T, template<typename> class SeparationPolicy>
class FileProcessor {
  std::string _file_name;
  SeparationPolicy<T> _policy;
  std::set<Domain> _domains;
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
      if (_current_domain.hash != 0) {
          _domains.insert(_current_domain);
          _current_domain = Domain();
        }
    }
    else {
      _current_domain.addLine(line);
    }
  }

  void finalizeCurrentDomain() {
    if (_current_domain.hash != 0) {
        _domains.insert(_current_domain);
        _current_domain = Domain();
    }
  }

  void clearDomains() noexcept { _domains.clear(); }

  std::set<Domain> getDomains() const noexcept { return _domains; }

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

class DomainManager {
  std::unordered_map<std::string, std::set<Domain>> _domains;
  std::unordered_map<std::size_t, std::string> domainToFile;
  std::set<Domain> _unique_domains;

public:
  DomainManager() = default;

  void addDomains(const std::string& fileName, const std::set<Domain>& domains) {
    _domains[fileName] = domains;
    for(const auto& domain : domains) {
      _unique_domains.insert(domain);
    }

    for (const auto& domain : domains) {
      domainToFile[domain.hash] = fileName;
    }
  }

  void printDomains() const {
    for(const auto& [file_name, domains] : _domains) {
      std::cout << file_name << " : " << std::endl;
      for(const auto& domain : domains) {
        std::cout << domain.hash << std::endl;
      }
    }
  }

  void printUniqueDomains() const {
    for(const auto& domain : _unique_domains) {
      std::cout << domain.hash << "   " << std::endl;
    }
  }

  const std::set<Domain>& getUniqueDomains() const noexcept { return _unique_domains; }

  const std::unordered_map<std::size_t, std::string>& getDomainToFile() const noexcept { return domainToFile; }

  const std::unordered_map<std::string, std::set<Domain>>& getDomains() const noexcept { return _domains; }
};


template<typename T, template<typename> class SeparationPolicy, class FileFilterPolicy>
class FileManager {
  FileProcessor<T, SeparationPolicy> _file;
  FileFilterPolicy _filter;
  std::string _path;

  DomainManager _domainManager;

  // std::unordered_map<std::string, std::vector<Domain>> _domains;
  // std::unordered_map<std::size_t, std::string> domainToFile;
  // std::set<Domain> _unique_domains;
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
        // _domains[dir.path().filename().string()] = domains;
        // for(const auto& domain : domains) {
        //   _unique_domains.insert(domain);
        // }

        // for (const auto& domain : domains) {
        //   domainToFile[domain.hash] = dir.path().filename().string();
        // }
      }
    }
  }

  const DomainManager& getDomainManager() const noexcept { return _domainManager; }
};

class SetCoverFast {
private:
  std::unordered_map<std::string, std::vector<std::set<std::size_t>>> file_sets;
  std::set<std::size_t> universe;

public:
  SetCoverFast(const std::unordered_map<std::string, std::vector<std::set<std::size_t>>>& file_sets, const std::set<std::size_t>& universe)
    : file_sets(file_sets), universe(universe) {}

  std::vector<std::string> solve() {
    std::vector<std::string> cover;
    while (!universe.empty()) {
      std::string best_file;
      std::set<std::size_t> best_file_cover;
      for (const auto& [file, sets] : file_sets) {
        std::set<std::size_t> union_set;
        for (const auto& set : sets) {
          union_set.insert(set.begin(), set.end());
        }
        std::set<std::size_t> intersection;
        std::set_intersection(union_set.begin(), union_set.end(), universe.begin(), universe.end(),
                              std::inserter(intersection, intersection.begin()));
        if (intersection.size() > best_file_cover.size()) {
          best_file = file;
          best_file_cover = intersection;
        }
      }
      if (best_file.empty()) {
        throw std::runtime_error("No valid set cover found");
      }
      cover.push_back(best_file);
      for (std::size_t element : best_file_cover) {
        universe.erase(element);
      }
    }
    return cover;
  }
};

int main(int argc, char* argv[]) {
  std::vector<std::string> args(argv, argv + argc);
  if(args.size() != 2) {
    std::cerr << "2 args required" << std::endl;
    return 1;
  }

  const std::string& path = args[1];

  CompositeFilterPolicy compositeFilter;
  //compositeFilter.addFilter(std::make_unique<NameContainsFilterPolicy>(""));
  compositeFilter.addFilter(std::make_unique<ExtensionFilterPolicy>(".css"));
  compositeFilter.addFilter(std::make_unique<NoHiddenFilesFilterPolicy>());

  FileManager<std::string, BlankLineSeparationPolicy, CompositeFilterPolicy> 
    manager(path, std::move(compositeFilter));
  manager.parseDomains();


  const auto& domains = manager.getDomainManager().getDomains();
  const auto& unique_domains = manager.getDomainManager().getUniqueDomains();
  const auto& domainToFile = manager.getDomainManager().getDomainToFile();

  //manager.getDomainManager().printDomains();

  std::unordered_map<std::string, std::vector<std::set<std::size_t>>> file_sets;
  std::set<std::size_t> universe;

  // Convert domains and unique_domains to file_sets and universe
  for (const auto& [file, domainSet] : domains) {
    std::vector<std::set<std::size_t>> sets;
    for (const auto& domain : domainSet) {
      sets.push_back({domain.hash});
    }
    file_sets[file] = sets;
  }

  for (const auto& domain : unique_domains) {
    universe.insert(domain.hash);
  }

  SetCoverFast setCover(file_sets, universe);

  std::vector<std::string> cover = setCover.solve();
  for (const auto& file : cover) {
    std::cout << file << std::endl;
  }
  std::cout << std::endl;
  return 0;
}

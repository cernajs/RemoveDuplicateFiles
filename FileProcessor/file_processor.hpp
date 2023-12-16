#ifndef FILE_PROCESSOR_HPP
#define FILE_PROCESSOR_HPP

#include <string>
#include <set>

#include "../Domain/domain.hpp"
#include "../SeparationPolicy/separation_policy.hpp"

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

#endif
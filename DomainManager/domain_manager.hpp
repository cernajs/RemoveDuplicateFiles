#ifndef DOMAIN_MANAGER_HPP
#define DOMAIN_MANAGER_HPP

#include <unordered_map>
#include <set>
#include <string>
#include <mutex>

#include "../Domain/domain.hpp"

class DomainManager {
  mutable std::mutex _mutex;

  std::unordered_map<std::string, std::set<Domain>> _domains;
  std::unordered_map<std::size_t, std::string> domainToFile;
  std::set<Domain> _unique_domains;

public:
    DomainManager() = default;

    void addDomains(const std::string& fileName, const std::set<Domain>& domains);

    void printDomains() const;

    void printUniqueDomains() const;

    const std::set<Domain>& getUniqueDomains() const noexcept { return _unique_domains; }

    const std::unordered_map<std::size_t, std::string>& getDomainToFile() const noexcept { return domainToFile; }

    const std::unordered_map<std::string, std::set<Domain>>& getDomains() const noexcept { return _domains; }
};

#endif // DOMAIN_MANAGER_HPP
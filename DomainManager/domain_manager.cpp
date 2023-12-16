#include "domain_manager.hpp"

#include <iostream>

void DomainManager::addDomains(const std::string& fileName, const std::set<Domain>& domains) {
    _domains[fileName] = domains;
    for(const auto& domain : domains) {
        _unique_domains.insert(domain);
    }

    for (const auto& domain : domains) {
        domainToFile[domain.hash] = fileName;
    }
}

void DomainManager::printDomains() const {
    for(const auto& [file_name, domains] : _domains) {
        std::cout << file_name << " : " << std::endl;
        for(const auto& domain : domains) {
        std::cout << domain.hash << std::endl;
        }
    }
}

void DomainManager::printUniqueDomains() const {
    for(const auto& domain : _unique_domains) {
        std::cout << domain.hash << "   " << std::endl;
    }
}
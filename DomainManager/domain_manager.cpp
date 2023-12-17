#include "domain_manager.hpp"

#include <iostream>

void DomainManager::addDomains(const std::string& fileName, const std::set<Domain>& domains) {
    std::lock_guard<std::mutex> lock(_mutex);
    _domains[fileName] = domains;
    for(const auto& domain : domains) {
        _unique_domains.insert(domain);
        domainToFile[domain.hash] = fileName;
    }
}

void DomainManager::printDomains() const {
    std::lock_guard<std::mutex> lock(_mutex);
    for(const auto& [file_name, domains] : _domains) {
        std::cout << file_name << " : " << std::endl;
        for(const auto& domain : domains) {
        std::cout << domain.hash << std::endl;
        }
    }
}

void DomainManager::printUniqueDomains() const {
    std::lock_guard<std::mutex> lock(_mutex);
    for(const auto& domain : _unique_domains) {
        std::cout << domain.hash << "   " << std::endl;
    }
}
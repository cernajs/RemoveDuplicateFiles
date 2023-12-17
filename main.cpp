#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <set>
#include <memory>

#include "fstream"
#include "filesystem"

#include <chrono>

#include "SeparationPolicy/separation_policies.hpp"
#include "FileFilterPolicy/file_filter_policies.hpp"

#include "FileProcessor/file_processor.hpp"

#include "Domain/domain.hpp"
#include "SetCover/set_cover.hpp"

#include "DomainManager/domain_manager.hpp"
#include "FileManager/file_manager.hpp"


int main(int argc, char* argv[]) {
  std::vector<std::string> args(argv, argv + argc);
  if(args.size() < 2) {
    std::cerr << "at least 2 arguments required : " << args[0] << " <directory-path>" << std::endl;
    return 1;
  }

  const std::string& path = args[1];

  CompositeFilterPolicy compositeFilter;

  for (size_t i = 0; i < args.size(); ++i) {
    if (args[i] == "--name-contains" && i + 1 < args.size()) {
        compositeFilter.addFilter(std::make_unique<NameContainsFilterPolicy>(args[++i]));
    } else if (args[i] == "--ends-with" && i + 1 < args.size()) {
        compositeFilter.addFilter(std::make_unique<ExtensionFilterPolicy>(args[++i]));
    } else if (args[i] == "--no-hidden-files") {
        compositeFilter.addFilter(std::make_unique<NoHiddenFilesFilterPolicy>());
    }
  }

  FileManager<std::string, BlankLineSeparationPolicy, CompositeFilterPolicy> 
    manager(path, std::move(compositeFilter));
  //auto start = std::chrono::high_resolution_clock::now();
  manager.parseDomains();
  //auto end = std::chrono::high_resolution_clock::now();
  //std::cout << "Time taken to parse domains: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

  const auto& domain_manager = manager.getDomainManager();
  const auto& domains = domain_manager->getDomains();
  const auto& unique_domains = domain_manager->getUniqueDomains();

  std::unordered_map<std::string, std::set<std::size_t>> file_sets;
  std::set<std::size_t> universe;
  for (const auto& [file, domainSet] : domains) {
    std::set<std::size_t> sets;
    for (const auto& domain : domainSet) {
      sets.insert(domain.hash);
      universe.insert(domain.hash);
    }
    file_sets[file] = sets;
  }

  SetCoverFast setCover(file_sets, universe);

  std::vector<std::string> cover = setCover.solve();

  for (const auto& file : cover) {
    std::cout << file << std::endl;
  }

  return 0;
}

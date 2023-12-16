#include "set_cover.hpp"

SetCoverFast::SetCoverFast(const std::unordered_map<std::string, std::set<std::size_t>>& file_sets, const std::set<std::size_t>& universe) 
    : file_sets(file_sets), universe(universe) {}

std::vector<std::string> SetCoverFast::solve() {
    std::vector<std::string> cover;

    while (!universe.empty()) {
        std::string best_set;
        std::size_t best_size = 0;

        for (const auto& file_set : file_sets) {
            std::set<std::size_t> intersection;
            std::set_intersection(universe.begin(), universe.end(), file_set.second.begin(), file_set.second.end(),
                                std::inserter(intersection, intersection.begin()));

            if (intersection.size() > best_size) {
                best_set = file_set.first;
                best_size = intersection.size();
            }
        }

        for (const auto& element : file_sets[best_set]) {
            universe.erase(element);
        }

        cover.push_back(best_set);
    }

    return cover;
}
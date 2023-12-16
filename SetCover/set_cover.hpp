#ifndef SETCOVERFAST_HPP
#define SETCOVERFAST_HPP

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>

class SetCoverFast {
private:
    std::unordered_map<std::string, std::set<std::size_t>> file_sets;
    std::set<std::size_t> universe;

public:
    SetCoverFast(const std::unordered_map<std::string, std::set<std::size_t>>& file_sets, const std::set<std::size_t>& universe);

    std::vector<std::string> solve();
};

#endif
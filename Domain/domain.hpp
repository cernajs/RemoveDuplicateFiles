#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <string>
#include <functional>

struct Domain {
    std::size_t hash;
    std::string filename;

    Domain() : hash(0) {}

    void addLine(const std::string& line) {
        std::size_t lineHash = std::hash<std::string>{}(line);
        hash ^= lineHash;
    }

    bool operator==(const Domain& other) const {
        return hash == other.hash;
    }

    bool operator<(const Domain& other) const {
        return hash < other.hash;
    }

    operator std::size_t() const {
        return hash;
    }
};

#endif // DOMAIN_HPP

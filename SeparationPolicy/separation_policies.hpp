#ifndef SEPARATION_POLICIES_HPP
#define SEPARATION_POLICIES_HPP

#include "separation_policy.hpp"

template<typename T>
struct BlankLineSeparationPolicy : SeparationPolicy<T> {
  bool splitContent(const T& line) override {
    return line.empty();
  }
};

template<typename T>
struct SeparationByCharPolicy : SeparationPolicy<T> {
  char _separator;

  SeparationByCharPolicy(char separator) : _separator(separator) {}

  bool splitContent(const T& line) override {
    return line.front() == _separator;
  }
};

#endif //SEPARATION_POLICIES_HPP
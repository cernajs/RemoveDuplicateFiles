#ifndef SEPARATION_POLICY_HPP
#define SEPARATION_POLICY_HPP

template<typename T>
struct SeparationPolicy {
  virtual bool splitContent(const T& line) = 0;
  virtual ~SeparationPolicy() = default;
};

#endif //SEPARATION_POLICY_HPP
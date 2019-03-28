#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>

namespace dvc {

inline std::string now_string() {
  std::ostringstream oss;
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  oss << std::put_time(std::localtime(&now_c), "%F %T");
  return oss.str();
}

inline uint64_t now() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

}  // namespace dvc

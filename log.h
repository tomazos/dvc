#pragma once

#include <iostream>

#include "dvc/string.h"

#define DVC_LOG(...)                                     \
  do {                                                   \
    ::dvc::info(__FILE__, ':', __LINE__,                 \
                ": info: ", ::dvc::concat(__VA_ARGS__)); \
  } while (0)

#define DVC_DUMP(expr) DVC_LOG(#expr, " = ", (expr));

#define DVC_ERROR(...)                                     \
  do {                                                     \
    ::dvc::error(__FILE__, ':', __LINE__,                  \
                 ": error: ", ::dvc::concat(__VA_ARGS__)); \
  } while (0)

#define DVC_FATAL(...)                                     \
  do {                                                     \
    ::dvc::fatal(__FILE__, ':', __LINE__,                  \
                 ": fatal: ", ::dvc::concat(__VA_ARGS__)); \
  } while (0)

#define DVC_FAIL(...)                                   \
  do {                                                  \
    ::dvc::fail("error: ", ::dvc::concat(__VA_ARGS__)); \
  } while (0)

#define DVC_ASSERT(condition, ...)                                  \
  DVC_FATAL_IF((condition), "assertion failed: ", #condition, ": ", \
               ::dvc::concat(__VA_ARGS__))

#define DVC_ASSERT_EQ(a, b, ...) \
  DVC_ASSERT_OP(==, a, b, ::dvc::concat(__VA_ARGS__))
#define DVC_ASSERT_NE(a, b, ...) \
  DVC_ASSERT_OP(!=, a, b, ::dvc::concat(__VA_ARGS__))
#define DVC_ASSERT_LT(a, b, ...) \
  DVC_ASSERT_OP(<, a, b, ::dvc::concat(__VA_ARGS__))
#define DVC_ASSERT_GT(a, b, ...) \
  DVC_ASSERT_OP(>, a, b, ::dvc::concat(__VA_ARGS__))
#define DVC_ASSERT_LE(a, b, ...) \
  DVC_ASSERT_OP(<=, a, b, ::dvc::concat(__VA_ARGS__))
#define DVC_ASSERT_GE(a, b, ...) \
  DVC_ASSERT_OP(>=, a, b, ::dvc::concat(__VA_ARGS__))

namespace dvc {

template <typename... Args>
void info(Args&&... args) {
  (std::cout << ... << std::forward<Args>(args));
  std::cout << std::endl;
}

template <typename... Args>
void error(Args&&... args) {
  (std::cerr << ... << std::forward<Args>(args));
  std::cerr << std::endl;
}

template <typename... Args>
[[noreturn]] void fatal(Args&&... args) {
  error(std::forward<Args>(args)...);
  std::terminate();
}

template <typename... Args>
[[noreturn]] void fail(Args&&... args) {
  error(std::forward<Args>(args)...);
  std::exit(EXIT_FAILURE);
}

#define DVC_FATAL_IF(condition, ...)                                           \
  do {                                                                         \
    if (!(condition))                                                          \
      ::dvc::fatal(__FILE__, ':', __LINE__, ": ", ::dvc::concat(__VA_ARGS__)); \
  } while (0)

#define DVC_ASSERT_OP(op, a, b, ...)                                          \
  DVC_FATAL_IF(((a)op(b)), "assertion failed: ", #a, ' ', #op, ' ', #b, ": ", \
               a, ' ', #op, ' ', b, ": ", ::dvc::concat(__VA_ARGS__))

}  // namespace dvc

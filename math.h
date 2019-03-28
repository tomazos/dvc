#pragma once

namespace dvc {

template <typename T>
constexpr bool is_pow2(T t) {
  return (t & (t - T(1))) == 0;
}

}  // namespace dvc

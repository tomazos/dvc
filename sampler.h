
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <random>
#include <vector>

#include "dvc/log.h"

namespace dvc {

template <typename T, size_t nsamples>
class sampler {
 public:
  sampler() : next_population(0) {}

  void operator()(T t) {
    size_t population = ++next_population;
    size_t pos = dis(ran, param(size_t(0), size_t(population - 1)));
    if (pos < nsamples) {
      std::lock_guard lock(mu);
      if (population <= nsamples)
        samples_[population] = std::pair(population - 1, t);
      else
        samples_[population] = std::pair(pos, t);
    }
  }

  size_t size() { return next_population; }

  std::vector<T> build_samples() {
    if (samples_.empty()) return {};
    std::vector<T> result(nsamples);
    for (const auto& [population, posnt] : samples_) {
      const auto& [pos, t] = posnt;
      result.at(pos) = t;
    }
    if (next_population < nsamples) result.resize(next_population);
    std::shuffle(result.begin(), result.end(), ran);
    return result;
  }

 private:
  std::ranlux48_base ran;
  std::uniform_int_distribution<size_t> dis;
  using param = typename decltype(dis)::param_type;
  std::atomic_size_t next_population;
  std::mutex mu;

  std::map<size_t, std::pair<size_t, T>> samples_;
};

}  // namespace dvc

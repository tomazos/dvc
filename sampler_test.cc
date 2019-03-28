#include "dvc/sampler.h"

#include "dvc/log.h"
#include "dvc/math.h"
#include "dvc/program.h"

int main(int argc, char** argv) {
  dvc::program program(argc, argv);

  dvc::sampler<size_t, 500> sampler;

  size_t n = (size_t(1) << 28);

  size_t start = dvc::now();
  for (size_t i = 0; i < n; i++) {
    sampler(i);
    //    if (dvc::is_pow2(i)) {
    //      DVC_DUMP(i);
    //      DVC_DUMP(sampler.samples_.size());
    //    }
  }
  size_t end = dvc::now();

  DVC_LOG((end - start) / n, "ns");

  std::vector<size_t> samples = sampler.build_samples();

  size_t total = 0;
  for (size_t sample : samples) {
    DVC_DUMP(sample);
    total += sample;
  }

  size_t avg = total / size_t(500);

  DVC_ASSERT_GT(avg, size_t(134217728) - size_t(10000000));
  DVC_ASSERT_LT(avg, size_t(134217728) + size_t(10000000));
}

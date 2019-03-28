#pragma once

#include <string>
#include <vector>

namespace dvc {

struct program {
  program();
  program(int& argc, char**& argv);
  ~program();
};

}  // namespace dvc

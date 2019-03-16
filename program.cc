#include "dvc/program.h"

#include "dvc/terminate.h"
#include "gflags/gflags.h"

namespace dvc {

program::program(int& argc, char**& argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  dvc::install_terminate_handler();
  args = std::vector<std::string>(argv + 1, argv + argc);
}

program::~program() { gflags::ShutDownCommandLineFlags(); }

}  // namespace dvc

#include "dvc/program.h"

#include "dvc/opts.h"
#include "dvc/terminate.h"

namespace dvc {

program::program() {
  dvc::install_terminate_handler();
}

program::program(int& argc, char**& argv) {
  dvc::init_options(argc, argv);
  dvc::install_terminate_handler();
}

program::~program() {}

}  // namespace dvc

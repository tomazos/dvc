#include "dvc/opts.h"

int DVC_OPTION(hello, H, 12, "test option");
std::vector<int> DVC_OPTION(hellos, s, dvc::required, "test option");
bool DVC_OPTION(foo, -, false, "test option");
bool DVC_OPTION(bar, b, false, "test option");
bool DVC_OPTION(baz, z, false, "test option");
std::vector<std::string> DVC_OPTION(stringz, S, {"foo", "bar"}, "test option");

int main(int argc, char** argv) {
  dvc::init_options(argc, argv);

  DVC_DUMP(hello);
  DVC_DUMP(foo);
  DVC_DUMP(bar);
  DVC_DUMP(baz);

  for (auto s : dvc::args) {
    std::cout << "ARG: " << s << std::endl;
  }

  for (auto s : stringz) {
    std::cout << "STRINGZ: " << s << std::endl;
  }
  for (auto s : hellos) {
    std::cout << "hellos: " << s << std::endl;
  }
}

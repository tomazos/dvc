#include "dvc/log.h"

#include "dvc/program.h"

int main(int argc, char** argv) {
  dvc::program program(argc, argv);

  DVC_LOG("hello world");
  DVC_ERROR("world is wrong");

  int foo = 2;
  int bar = 3;
  DVC_DUMP(foo);
  DVC_DUMP(bar);
  DVC_ASSERT_EQ(foo, foo, "goodbye world");
  DVC_ASSERT_NE(foo, bar, "goodbye world");
  DVC_ASSERT_LT(foo, bar, "goodbye world");
  DVC_ASSERT_LE(foo, bar, "goodbye world");
  DVC_ASSERT_GT(bar, foo, "goodbye world");
  DVC_ASSERT_GE(bar, foo, "goodbye world");
}

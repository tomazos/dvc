#include "dvc/string.h"

#include "dvc/log.h"

int main() {
  int x;
  DVC_ASSERT(dvc::destring("42", x));
  DVC_ASSERT_EQ(x, 42);
}

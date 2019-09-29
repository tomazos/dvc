#include "dvc/python.h"

int main() {
  Py_Initialize();

  Py_CompileString("foo", "bar", Py_eval_input);

  Py_Finalize();
}

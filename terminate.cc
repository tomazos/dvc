#include "dvc/terminate.h"

#include <signal.h>

#include <iostream>

#include "dvc/log.h"

#define BOOST_STACKTRACE_USE_BACKTRACE 1
#include <boost/stacktrace.hpp>

namespace dvc {

void log_stacktrace() { error(boost::stacktrace::stacktrace()); }

void log_current_exception() {
  std::exception_ptr exception_ptr = std::current_exception();
  try {
    if (exception_ptr) std::rethrow_exception(exception_ptr);
  } catch (const std::exception& e) {
    error("exception: ", e.what());
  }
}

void terminate_handler() {
  log_stacktrace();
  log_current_exception();
}

void install_terminate_handler() { std::set_terminate(terminate_handler); }

void fatal_signal_handler(int signal) {
  (void)signal;
  dvc::error("segmentation fault");
  log_stacktrace();
  std::abort();
}

void install_segfault_handler() {
  struct sigaction sa = {};
  sa.sa_handler = fatal_signal_handler;
  sigaction(SIGSEGV, &sa, nullptr);
}

}  // namespace dvc

#include "timestamp.h"
#include <iostream>

void _timestamp(const size_t line, const std::string file) {
  static auto then = std::chrono::high_resolution_clock::now();
  const auto now   = std::chrono::high_resolution_clock::now();

  std::cout << file << ':' << line << '\t' << (now - then).count() << '\n';

  then = now;
}

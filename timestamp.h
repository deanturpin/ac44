#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <chrono>
#include <fstream>
#include <iostream>

auto init() { return std::chrono::high_resolution_clock::now(); }

void timestamp(const size_t line, const std::string file = "undef") {
  static auto then = init();
  auto &out        = std::cout;

  const auto now = std::chrono::high_resolution_clock::now();

  out << file << ':' << line << '\t' << (now - then).count() << '\n';

  then = now;
}

#endif

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <chrono>
#include <fstream>
#include <iostream>

auto init() { return std::chrono::system_clock::now(); }

void timestamp(const size_t line, const std::string file = "undef") {
  static const auto then = init();
  // static std::ofstream out("tmp/perf.csv");
  auto &out = std::cout;

  const auto now = std::chrono::system_clock::now();

  out << file << ':' << line << '\t' << (now - then).count() << '\n';

  // if (end_of_line)
  //   out << '\n';
}

#endif

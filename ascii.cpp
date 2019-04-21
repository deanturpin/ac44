#include "ascii.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>

// Create text representation of basic histogram
std::string dump_histogram(const iterator_t &begin, const iterator_t &end) {

  // Calculate max bin so we can scale the output
  const double max_bin{*std::max_element(begin, end)};

  // Max width of a bar
  const size_t max_width = 370;

  std::ostringstream out;
  out << '\n';
  std::for_each(begin, end, [&](const auto &bin) {
    out << std::string(1 + std::rint(max_width * bin / max_bin), '#')
        << "\033[36m_\033[0m\n";
  });

  return out.str();
}

// Routine that takes a histogram and a function to scale it and returns the
// scaled histogram
using cont_t = std::vector<double>;
cont_t scale_histogram(const iterator_t &begin, const iterator_t &end,
                       std::function<size_t(size_t)> func) {

  // Construct scaled histogram
  std::vector<double> scaled_histogram;
  scaled_histogram.reserve(std::distance(begin, end));

  std::for_each(begin, end, [&, n = 0](const auto &s) mutable {
    // size_t index = std::rint(std::log2(n * 1024 * 1024));
    size_t index = func(n);

    // If it's a new bin create it
    if (index >= scaled_histogram.size())
      scaled_histogram.emplace_back();

    // Sum all the values that match a bin
    scaled_histogram.back() += s;

    ++n;
  });

  // Free up any unused memory
  scaled_histogram.shrink_to_fit();

  return scaled_histogram;
}

// Create text representation of histogram with a log X axis
std::string dump_log_histogram(const iterator_t &begin, const iterator_t &end) {

  const auto scaled = scale_histogram(begin, end, [](const auto &n) {
    return std::rint(std::log2(n * 1024 * 1024));
  });

  // Pass log histogram to basic histogram routine
  return dump_histogram(std::cbegin(scaled), std::cend(scaled));
}

// Create text representation of log histogram from above
std::string dump_aerial_histogram(const iterator_t &begin,
                                  const iterator_t &end) {

  // Construct scaled histogram
  const auto scaled = scale_histogram(begin, end, [&](const auto &n) {
    return std::rint(85.0 * n / std::distance(begin, end));
  });

  // Max length of a bar
  const std::string chars{" _.oO#"};
  const size_t max_length = chars.size() - 1;

  // Calculate max bin so we can scale the output
  const double max_bin{
      *std::max_element(std::cbegin(scaled), std::cend(scaled))};

  std::ostringstream out;
  std::for_each(std::cbegin(scaled), std::cend(scaled), [&](const auto &bin) {
    out << chars.at(std::floor(max_length * bin / max_bin));
  });
  out << '\n';

  return out.str();
}

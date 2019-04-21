#include "ascii.h"
#include <algorithm>
#include <cmath>
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

// Create text representation of histogram with a log X axis
std::string dump_log_histogram(const iterator_t &begin, const iterator_t &end) {

  // Construct log histogram
  std::vector<double> log_histogram;
  log_histogram.reserve(std::distance(begin, end));

  std::for_each(begin, end, [&, n = 0](const auto &s) mutable {
    size_t index = std::rint(std::log2(n * 1024 * 1024));

    // If it's a new bin create it
    if (index >= log_histogram.size())
      log_histogram.emplace_back();

    // Sum all the values that match a bin
    log_histogram.back() += s;

    ++n;
  });

  // Free up any unused memory
  log_histogram.shrink_to_fit();

  // Pass log histogram to basic histogram routine
  return dump_histogram(std::cbegin(log_histogram), std::cend(log_histogram));
}

// Create text representation of log histogram from above
std::string dump_aerial_histogram(const iterator_t &begin,
                                  const iterator_t &end) {

  // Construct scaled histogram
  std::vector<double> scaled;
  scaled.reserve(std::distance(begin, end));

  std::for_each(begin, end, [&, n = 0](const auto &s) mutable {
    size_t index = std::rint(85.0 * n / std::distance(begin, end));

    // If it's a new bin create it
    if (index >= scaled.size())
      scaled.emplace_back();

    // Sum all the values that match a bin
    scaled.back() += s;

    ++n;
  });

  // Free up any unused memory
  scaled.shrink_to_fit();

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

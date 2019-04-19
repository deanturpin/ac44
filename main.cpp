#include "fourier.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// Structure of a WAV
struct ac44 {
  std::uint32_t riff_id{};
  std::uint32_t riff_size{};
  std::uint32_t wave_tag{};
  std::uint32_t format_id{};
  std::uint32_t format_size{};
  std::uint16_t format_tag{};
  std::uint16_t channels{};
  std::uint32_t sample_rate{};
  std::uint32_t bytes_per_second{};
  std::uint16_t block_align{};
  std::uint16_t bit_depth{};
  std::uint32_t data_id{};
  std::uint32_t data_size{};
};

// Read WAV header from a stream and return sample rate
ac44 get_meta(std::istream &in) {

  ac44 meta;
  assert(sizeof(meta) == 44);

  // Check file is good and read the meta data
  if (in.good())
    in.read(reinterpret_cast<char *>(&meta), sizeof meta);

  return meta;
}

// Create text representation of basic histogram
using iterator_t = std::vector<double>::const_iterator;
std::string dump_histogram(const iterator_t &begin, const iterator_t &end) {

  // Calculate max bin so we can scale the output
  const double max_bin{std::log2(*std::max_element(begin, end))};

  // Max width of a bar
  const size_t max_width = 80;

  std::ostringstream out;
  out << '\n';
  std::for_each(begin, end, [&](const auto &bin) {
    out << std::string(1 + std::rint(max_width * std::log2(bin) / max_bin), '_')
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
      log_histogram.push_back(0.0);

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
    size_t index = std::rint(174 * n / fourier_bins);

    // If it's a new bin create it
    if (index >= scaled.size())
      scaled.push_back(0.0);

    // Sum all the values that match a bin
    scaled.back() += s;

    ++n;
  });

  // Free up any unused memory
  scaled.shrink_to_fit();

  // Max length of a bar
  const size_t max_length = 10;

  // Calculate max bin so we can scale the output
  const double max_bin{
      *std::max_element(std::cbegin(scaled), std::cend(scaled))};

  std::ostringstream out;
  std::for_each(std::cbegin(scaled), std::cend(scaled), [&](const auto &bin) {
    out << std::floor(max_length * bin / max_bin);
  });
  out << '\n';

  return out.str();
}

int main() {

  // Define source of audio
  auto &in = std::cin;

  // Get the meta data
  const auto &sample_rate = get_meta(in).sample_rate;

  // Prepare container to read a batch of samples
  std::vector<sample_t> samples(sample_rate / 8);
  const size_t bytes_in_batch{samples.size() * sizeof(sample_t)};

  // Repeatedly read batches of samples and report stats until read fails
  while (in.read(reinterpret_cast<char *>(samples.data()), bytes_in_batch)) {

    // Get Fourier transform for this batch and dump it
    const auto &fourier = get_fourier(samples);
    std::cout << dump_aerial_histogram(std::cbegin(fourier),
                                       std::cend(fourier));
  }
}

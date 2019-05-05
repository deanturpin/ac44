#include "fourier.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iostream>
#include <iterator>
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

int main() {

  // Define source of audio
  auto &in = std::cin;

  // Get the meta data
  const auto &sample_rate = get_meta(in).sample_rate;
  const size_t scale      = 1;

  // Prepare container to read a batch of samples
  std::vector<int16_t> samples(sample_rate);
  const size_t bytes_in_batch{samples.size() * sizeof(int16_t) / scale};

  // Repeatedly read batches of samples and report stats until read fails
  while (in.read(reinterpret_cast<char *>(samples.data()), bytes_in_batch)) {

    static std::deque<double> harmonics;

    // Get Fourier transform for this batch and dump it
    const auto &fourier = get_fourier(samples);

    const size_t bin = std::distance(
        fourier.cbegin(), std::max_element(fourier.cbegin(), fourier.cend()));

    // Store current bin
    const auto frequency = scale * bin * samples.size() / fourier.size();

    if (frequency > 0)
      harmonics.push_back(frequency);

    // Drop the duplicates
    auto last = std::unique(harmonics.begin(), harmonics.end());
    harmonics.erase(last, harmonics.end());

    // Dump the oldest
    if (harmonics.size() > 1)
      harmonics.pop_front();

    // Report bins
    std::stringstream out;
    std::copy(harmonics.cbegin(), harmonics.cend(),
              std::ostream_iterator<double>(out, "\t"));

    if (!harmonics.empty())
      out << '\n';

    // Dump to stdout and std err
    std::cout << out.str();
    std::cerr << out.str();
  }
}

#include "fourier.h"
#include <array>
#include <cassert>
#include <cstdint>
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

  // Prepare container to read a batch of samples
  std::vector<int16_t> samples(sample_rate / 10);
  const size_t bytes_in_batch{samples.size() * sizeof(int16_t)};

  // Repeatedly read batches of samples and report stats until read fails
  while (in.read(reinterpret_cast<char *>(samples.data()), bytes_in_batch)) {

    // Get Fourier transform for this batch and dump it
    const auto &fourier = get_fourier(samples);

    std::copy(std::cbegin(fourier), std::cend(fourier),
              std::ostream_iterator<double>(std::cout, "\n"));

    std::cout << "\n";
  }
}

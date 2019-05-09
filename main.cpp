#include "fourier.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iostream>
#include <future>
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

    // Create placeholder for future
    std::future<std::vector<double>> fourier;

  for (size_t i = 0; i < 40; ++i) {

    // Read batch of pulses
    std::vector<int16_t> samples(sample_rate / 2);
    std::cin.read(reinterpret_cast<char *>(samples.data()), samples.size() * sizeof(int16_t));
    // Fetch results from previous batch (if it exists)
    if (fourier.valid()) {

      // The future is bright
      const auto f = fourier.get();

      // Dump max bin frequency
      const auto max = std::distance(f.cbegin(), std::max_element(f.cbegin(), f.cend()));
      const auto bins = f.size();
      std::cout << sample_rate * max / bins << "\n";
    }
    
    // Initiate next analysis
    fourier = std::async(std::launch::async, get_fourier, samples);
  }

  // Promises promises...
  fourier.get();
}

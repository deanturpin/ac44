#include "fourier.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <deque>
#include <future>
#include <iostream>
#include <iterator>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
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

  // Create a large block of samples
  const size_t duration{10};
  std::vector<int16_t> samples(sample_rate * duration);

  // Split into blocks
  const size_t blocks_per_second{10};
  const size_t block_size{sample_rate / blocks_per_second};

  std::atomic<std::int16_t> latest;
  std::mutex quick, slow;
  std::atomic<bool> shutdown;

  const auto quick_loop = [&]() {
    while (!shutdown.load()) {

      // Wait until some samples are available
      quick.lock();

      // Prepare to copy the latest block
      const auto i = latest.load();
      const size_t start = i * block_size;
      const size_t end = (i + 1) * block_size;
      const std::vector<int16_t> s(&samples[start], &samples[end]);

      // Do Fourier analysis
      const auto f = get_fourier(s);

      // Calculate frequency of max bin
      const auto peak_bin =
          std::distance(f.cbegin(), std::max_element(f.cbegin(), f.cend()));

      // Size of Fourier response
      const auto bins = f.size();

      std::cout << sample_rate * peak_bin / bins << " quick\n";
    }
  };

  const auto slow_loop = [&]() {
    while (!shutdown.load()) {

      // Wait until some samples are available
      slow.lock();

      // Prepare to copy the latest block
      const auto i = latest.load();
      const size_t start = i * block_size;
      const size_t end = (i + 1) * block_size;
      const std::vector<int16_t> s(&samples[start], &samples[end]);

      // Do Fourier analysis
      const auto f = get_fourier(s);

      // Calculate frequency of max bin
      const auto peak_bin =
          std::distance(f.cbegin(), std::max_element(f.cbegin(), f.cend()));

      // Size of Fourier response
      const auto bins = f.size();

      std::cout << sample_rate * peak_bin / bins << " slow\n";
    }
  };

  // Get ready to release thread
  shutdown.store(false);
  quick.lock();
  slow.lock();
  std::thread t1(quick_loop);
  std::thread t2(slow_loop);

  for (size_t i = 0; i < samples.size() / block_size; ++i) {

    // Read a block of samples
    std::cin.read(reinterpret_cast<char *>(&samples[i * block_size]),
                  block_size * sizeof(int16_t));

    // Store current index and release the processing
    latest.store(i);
    quick.unlock();

    if (!((i + 1) % blocks_per_second))
      slow.unlock();
  }

  // Wait for threads to return
  shutdown.store(true);
  t1.join();
  t2.join();
}

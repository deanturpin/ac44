#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// Read WAV header from a stream and return sample rate
uint32_t get_sample_rate(std::istream &in) {

  // Structure of a WAV
  struct ac44 {
    uint32_t riff_id{};
    uint32_t riff_size{};
    char wave_tag[4]{};
    uint32_t format_id{};
    uint32_t format_size{};
    uint16_t format_tag{};
    uint16_t channels{};
    uint32_t sample_rate{};
    uint32_t bytes_per_second{};
    uint16_t block_align{};
    uint16_t bit_depth{};
    uint32_t data_id{};
    uint32_t data_size{};
  } meta;

  assert(sizeof(meta) == 44);

  // Check file is good and read the meta data
  if (in.good())
    in.read(reinterpret_cast<char *>(&meta), sizeof meta);

  return meta.sample_rate;
}

// Report summary of batch by splitting into blocks
using sample_t   = int16_t;
using iterator_t = std::vector<sample_t>::const_iterator;
std::string report(const iterator_t &begin, const iterator_t &end) {

  std::stringstream out;

  const size_t x = 3000;
  for (auto i = begin; i < std::prev(end, x); i += x) {
    const auto average_amplitude =
        std::accumulate(
            i, std::next(i, x), 0,
            [](auto &sum, const auto &a) { return sum += std::abs(a); }) /
        x;

    // Find the peak so we can scale the output
    static int16_t max_so_far = 1;
    max_so_far = std::max(max_so_far, *std::max_element(begin, end));

    // Calculate bar length for this bin
    const size_t max_bar_length = 75;
    const size_t bar_length = max_bar_length * average_amplitude / max_so_far;

    // Construct histogram bar and mark if it's clipped, always add one so we
    // don't attempt to constrcut a zero length string
    out << std::string(1 + bar_length, '-') << '\n';

    // Fade the max scale
    max_so_far = std::max(max_so_far - 300, 1);
  }

  return out.str();
}

int main() {

  // Read the header
  auto &in                = std::cin;
  const auto &sample_rate = get_sample_rate(in);

  // Read batches of samples
  std::vector<sample_t> s(sample_rate);
  const size_t count = s.size();
  const size_t size  = sizeof(sample_t);
  while (in.read(reinterpret_cast<char *>(s.data()), count * size))
    std::cout << report(std::cbegin(s), std::cend(s));
}

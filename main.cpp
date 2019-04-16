#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
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

int main() {

  // Read the header
  auto &in                = std::cin;
  const auto &sample_rate = get_sample_rate(in);

  // Read batches of samples
  std::vector<int16_t> s(sample_rate);
  while (in.read(reinterpret_cast<char *>(s.data()),
                 s.size() * sizeof(decltype(s)::value_type))) {

    // Report summary of batch by splitting into blocks
    const size_t x = 4000;
    for (auto i = std::cbegin(s); i < std::prev(std::cend(s), x); i += x) {
      const auto average_amplitude =
          std::accumulate(
              i, std::next(i, x), 0,
              [](auto sum = 0, const auto &a) { return sum += std::abs(a); }) /
          x;

      // Start and end for current block
      const auto begin = i;
      const auto end   = std::prev(std::cend(s), x);

      // Find the peak so we can scale the output
      // static int16_t max_so_far = 0; // std::numeric_limits<int16_t>::max();
      // const auto max_so_far = std::max(max_so_far, *std::max_element(begin,
      // end));
      const auto max_so_far = *std::max_element(begin, end);

      // Calculate bar length for this bin
      const size_t max_bar_length = 65;
      const size_t bar_length = max_bar_length * average_amplitude / max_so_far;

      // Construct histogram bar and mark if it's clipped, always add one so we
      // don't attempt to constrcut a zero length string
      std::cout << std::string(1 + bar_length, '-') << '|' << '\n';
    }
  }
}

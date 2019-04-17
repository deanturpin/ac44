#include <algorithm>
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

// void write_meta(std::ostream &out, const ac44 &meta) {
//
//   // Check file is good and read the meta data
//   if (out.good())
//     out.write(reinterpret_cast<char *>(&meta), sizeof meta);
// }

// Report summary of batch by splitting into blocks
using sample_t   = int16_t;
using iterator_t = std::vector<sample_t>::const_iterator;
std::string report(const iterator_t &begin, const iterator_t &end) {

  std::ostringstream out;

  const std::ptrdiff_t x = 3000;
  for (auto i = begin; i < std::prev(end, x); i += x) {
    const auto average_amplitude =
        std::accumulate(
            i, std::next(i, x), 0,
            [](auto &sum, const auto &a) { return sum += std::abs(a); }) /
        x;

    // Find the peak so we can scale the output
    static int16_t max_so_far = 1;
    const int16_t new_max = std::max(max_so_far, *std::max_element(begin, end));

    // Calculate bar length for this bin
    const std::size_t max_bar_length = 75;
    const std::size_t bar_length =
        max_bar_length * average_amplitude / max_so_far;

    // Construct histogram bar and mark if it's clipped, always add one so we
    // don't attempt to constrcut a zero length string
    out << std::string(std::clamp(bar_length, 1ul, max_bar_length), '-')
        << (bar_length > max_bar_length ? "<" : "") << '\n';

    // Fade the max scale
    max_so_far = std::max(new_max - 300, 1);
  }

  return out.str();
}

int main() {

  // Define source of audio
  auto &in  = std::cin;
  auto &out = std::cout;

  // Get the meta data
  ac44 meta = get_meta(in);
  // const auto &sample_rate = meta.sample_rate;

  // Emit the header
  out.write(reinterpret_cast<char *>(&meta), sizeof meta);

  // Prepare container to read a batch of samples
  std::vector<sample_t> samples(1000);
  const size_t bytes_in_batch{samples.size() * sizeof(sample_t)};

  // Repeatedly read batches of samples and report stats until read fails
  while (in.read(reinterpret_cast<char *>(samples.data()), bytes_in_batch)) {

    // std::cerr << report(std::cbegin(samples), std::cend(samples));

    // Invert the samples
    for (auto &s : samples)
      s = -s;

    // Emit the audio
    out.write(reinterpret_cast<char *>(&samples[0]), bytes_in_batch);
  }
}

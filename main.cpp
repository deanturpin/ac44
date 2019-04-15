#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

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
};

// Dump a WAV header
auto dump_meta(const ac44 &meta) {
  std::ostringstream out;
  out << std::hex << meta.wave_tag[0] << meta.wave_tag[1] << meta.wave_tag[2]
      << meta.wave_tag[3] << '\n'
      << std::hex << meta.sample_rate << " sample rate\n"
      << std::hex << meta.data_size << " bytes of samples\n"
      << std::dec << meta.channels << " channel"
      << (meta.channels > 1 ? "s" : "") << '\n';

  return out.str();
}

// Read WAV header from a stream
auto get_meta(std::istream &in) {

  ac44 meta;
  assert(sizeof(meta) == 44);

  // Check file is good and read the meta data
  if (in.good())
    in.read(reinterpret_cast<char *>(&meta), sizeof meta);

  return meta;
}

// Return a simple histogram of the samples passed in
using sample_t   = int16_t;
using iterator_t = const std::vector<sample_t>::const_iterator;
std::string create_display_histogram(iterator_t &begin, iterator_t &end) {

  // Create display histogram
  const size_t bin_count{37};
  const size_t bin_width{std::distance(begin, end) / bin_count};
  std::map<size_t, int64_t> hist;

  // Populate display histogram by truncating the pulse index to fit it on
  // the screen
  for (auto i = begin; i != end; ++i)
    hist[std::distance(begin, i) / bin_width] += *i;

  // Report histogram
  std::stringstream out;
  for (const auto &[bin, value] : hist) {

    // Calculate bar length for this bin
    const size_t max_bar_length = 70;
    const size_t bar_length =
        std::abs(value) / std::numeric_limits<int16_t>::max();

    // Construct histogram bar and mark if it's clipped
    const std::string bar = bar_length > max_bar_length
                                ? std::string(max_bar_length, '-') + "|"
                                : std::string(bar_length, '-');

    out << bar << '\n';
  }

  return out.str();
}

int main() {

  // Read the header
  auto &in         = std::cin;
  const auto &meta = get_meta(in);

  // Read batches of samples
  std::vector<sample_t> samples(meta.sample_rate);
  while (in.read(reinterpret_cast<char *>(samples.data()),
                 samples.size() * sizeof(sample_t))) {

    using clock = std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::microseconds;

    // Start timer
    const auto start = clock::now();

    // Calculate peak sample
    const decltype(samples)::value_type peak =
        std::abs(*std::max_element(std::cbegin(samples), std::cend(samples),
                                   [](const auto &a, const auto &b) {
                                     return std::abs(a) < std::abs(b);
                                   }));

    // Stop timer and report stats
    const auto end  = clock::now();
    const auto diff = duration_cast<microseconds>(end - start);
    std::cout << dump_meta(meta) << peak << '\t' << diff.count() << " us\n"
              << create_display_histogram(std::cbegin(samples),
                                          std::cend(samples));
    ;
  }
}

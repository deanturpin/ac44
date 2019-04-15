#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
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

// Return a simple histogram of the samples passed in
using sample_t   = int16_t;
using iterator_t = const std::vector<sample_t>::const_iterator;
std::string create_display_histogram(iterator_t &begin, iterator_t &end) {

  // Create display histogram
  const size_t bin_count{41};
  const size_t bin_width{std::distance(begin, end) / bin_count};
  std::map<size_t, uint64_t> hist;

  // Populate display histogram by truncating the pulse index to fit it on
  // the screen
  for (auto i = begin; i != end; ++i)
    hist[std::distance(begin, i) / bin_width] += std::abs(*i);

  // Calculate max histogram bin to scale the display
  const auto peak = std::max_element(std::cbegin(hist), std::cend(hist),
                                     [](const auto &a, const auto &b) {
                                       return a.second < b.second;
                                     })
                        ->second;

  // Report histogram
  std::stringstream out;
  for (const auto &[bin, value] : hist) {

    // Calculate bar length for this bin
    const size_t max_bar_length = 70;
    const size_t bar_length     = max_bar_length * value / peak;

    // Construct histogram bar and mark if it's clipped, always add one so we
    // don't attempt to constrcut a zero length string
    out << std::string(1 + bar_length, '-') << '\n';
  }

  return out.str();
}

int main() {

  // Read the header
  auto &in                = std::cin;
  const auto &sample_rate = get_sample_rate(in);

  // Read batches of samples
  std::vector<sample_t> s(sample_rate);
  while (in.read(reinterpret_cast<char *>(s.data()),
                 s.size() * sizeof(sample_t))) {

    using clock = std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::microseconds;

    // Start timer
    const auto start = clock::now();

    // Calculate peak sample
    const auto peak = std::abs(*std::max_element(
        std::cbegin(s), std::cend(s), [](const auto &a, const auto &b) {
          return std::abs(a) < std::abs(b);
        }));

    // Calculate bit template
    sample_t id{};
    std::for_each(std::cbegin(s), std::cend(s),
                  [&](const auto &sample) { id |= std::abs(sample); });

    // Stop timer and report stats
    const auto end  = clock::now();
    const auto diff = duration_cast<microseconds>(end - start);
    std::cout << peak << '\t' << std::bitset<16>(id) << '\t' << diff.count()
              << " us\n"
              << create_display_histogram(std::cbegin(s), std::cend(s));
  }
}

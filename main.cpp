#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

struct ac44 {
  uint32_t riff_id;
  uint32_t riff_size;
  char wave_tag[4];
  uint32_t format_id;
  uint32_t format_size;
  uint16_t format_tag;
  uint16_t channels;
  uint32_t sample_rate;
  uint32_t bytes_per_second;
  uint16_t block_align;
  uint16_t bit_depth;
  uint32_t data_id;
  uint32_t data_size;
};

auto dump_meta(const ac44 &meta) {
  std::ostringstream out;
  out << std::hex << meta.wave_tag[0] << meta.wave_tag[1] << meta.wave_tag[2]
      << meta.wave_tag[3] << '\n'
      << std::hex << meta.sample_rate << " sample rate\n"
      << std::hex << meta.data_size << " bytes of samples\n"
      << std::dec << meta.channels << " channel"
      << (meta.channels > 1 ? "s" : "") << "\n---\n";

  return out.str();
}

auto get_meta(std::istream &in) {

  ac44 meta;
  assert(sizeof(meta) == 44);

  // Check file is good and read the meta data
  if (in.good())
    in.read(reinterpret_cast<char *>(&meta), sizeof meta);

  return meta;
}

int main() {

  // Read a batch of samples
  using sample_t = int16_t;
  std::vector<sample_t> samples(44100 / 1);
  const size_t bins{20u};

  const auto &meta = get_meta(std::cin);
  std::cout << dump_meta(meta);

  while (std::cin.read(reinterpret_cast<char *>(samples.data()),
                       samples.size() * sizeof(sample_t))) {

    const auto &[min, max] =
        std::minmax_element(std::cbegin(samples), std::cend(samples));

    const size_t bin_width = samples.size() / bins;

    std::cout << *min << '\t' << *max << '\t' << bin_width << '\n';

    // Populate display histogram
    std::map<size_t, uint32_t> hist;
    for (auto i = std::cbegin(samples); i != std::cend(samples); ++i)
      hist[std::distance(std::cbegin(samples), i) / bin_width] += *i;

    // Report histogram
    for (const auto &[bin, value] : hist)
      std::cout << bin << '\t' << std::bitset<32>(abs(value)) << '\n';
  }
}

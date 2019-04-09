#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

std::string get_header(std::istream &in) {

  struct {
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
  } header;

  assert(sizeof(header) == 44);

  std::stringstream out;

  // Check file is good
  if (std::cin.good()) {

    // Read the header
    in.read(reinterpret_cast<char *>(&header), sizeof header);

    out << std::hex << header.wave_tag[0] << header.wave_tag[1]
        << header.wave_tag[2] << header.wave_tag[3] << '\n'
        << std::hex << header.sample_rate << " sample rate\n"
        << std::hex << header.data_size << " bytes of samples\n"
        << std::dec << header.channels << " channel"
        << (header.channels > 1 ? "s" : "") << '\n';
  }

  return out.str();
}

int main() {

  // Read a batch of samples
  using sample_t = int16_t;
  std::vector<sample_t> samples(44100 / 2);

  std::cout << get_header(std::cin);

  while (std::cin.read(reinterpret_cast<char *>(samples.data()),
                       samples.size() * sizeof(sample_t))) {

    const auto &[min, max] =
        std::minmax_element(std::cbegin(samples), std::cend(samples));

    const auto &mean =
        std::accumulate(std::cbegin(samples), std::cend(samples), 0.0) /
        samples.size();

    std::cout << *min << '\t' << *max << '\t' << mean << '\n';
  }
}

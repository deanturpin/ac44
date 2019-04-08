#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

int main() {

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

  // Check file is good
  if (std::cin.good()) {

    // Read the header
    std::cin.read(reinterpret_cast<char *>(&header), sizeof header);

    std::cout << std::hex << std::showbase << header.riff_id << '\n'
              << header.riff_size << '\n'
              << header.wave_tag[0] << header.wave_tag[1] << header.wave_tag[2]
              << header.wave_tag[3] << '\n'
              << std::hex << header.sample_rate << " bytes of samples\n"
              << std::hex << header.data_size << " bytes of samples\n"
              << std::dec << header.channels << " channel"
              << (header.channels > 1 ? "s" : "") << '\n';
  }

  // Read a batch of samples
  const size_t count{44100};

  using sample_t = int16_t;
  std::vector<sample_t> samples(count);

  int iterations{};

  while (iterations++ < 100) {

    std::cin.read(reinterpret_cast<char *>(samples.data()),
                  samples.size() * sizeof(sample_t));

    const auto &[min, max] =
        std::minmax_element(std::cbegin(samples), std::cend(samples));

    std::cout << "min/max\t" << *min << "\t" << *max << "\n";
  }
}

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main() {

  struct {
    uint32_t riff_id;
    uint32_t riff_size;
    uint32_t wave_tag;
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

  const std::string file{"../dft/wav/KP_guest.wav"};

  if (std::ifstream in{file}; in.good()) {

    in.read(reinterpret_cast<char *>(&header), sizeof header);

    std::cout << std::hex << std::showbase << header.riff_id << '\n'
              << header.riff_size << '\n'
              << header.wave_tag << '\n'
              << std::dec << header.data_size << " bytes\n"
              << header.channels << " channels\n";

    // Create a container for the samples
    using sample_t = int16_t;
    std::vector<sample_t> samples(header.data_size);
    in.read(reinterpret_cast<char *>(samples.data()),
            samples.size() * sizeof(sample_t));

    std::cout << samples.size() << " samples read\n";

    // Preview samples if we have some
    const size_t sample_preview{267};
    if (samples.size() >= sample_preview)
      std::copy_n(std::cbegin(samples), sample_preview,
                  std::ostream_iterator<sample_t>(std::cout, "\n"));
  } else
    std::cout << file << " is bad\n";

  // TODO - normalise
}

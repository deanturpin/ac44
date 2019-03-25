#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
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

  const std::string file{"../dft/wav/singing_bowl5.wav"};

  std::ostringstream out;

  // Check file is good
  if (std::ifstream in{file}; in.good()) {

    // Read only the header
    in.read(reinterpret_cast<char *>(&header), sizeof header);

    out << std::hex << std::showbase << header.riff_id << '\n'
        << header.riff_size << '\n'
        << header.wave_tag << '\n'
        << std::dec << header.data_size << " bytes of samples\n"
        << header.channels << " channel" << (header.channels > 1 ? "s" : "")
        << '\n';

    // Create a container for the samples
    using sample_t = int16_t;
    std::vector<sample_t> samples(header.data_size / sizeof(sample_t));
    in.read(reinterpret_cast<char *>(samples.data()),
            samples.size() * sizeof(sample_t));

    out << samples.size() << " samples read\n";

    // Preview samples if we have some
    const size_t preview_size{30};

    if (samples.size() >= preview_size)
      std::copy_n(std::cbegin(samples), preview_size,
                  std::ostream_iterator<sample_t>(out, "\n"));

    // Dump to a CSV
    if (std::ofstream csv{"tmp/blah.csv"}; csv.good())
      std::copy(std::cbegin(samples), std::cend(samples),
                std::ostream_iterator<sample_t>(csv, "\n"));

  } else
    out << file << " is bad\n";

  std::cout << out.str();

  // TODO - normalise
}

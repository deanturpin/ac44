#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
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

  std::ostringstream out;

  // Read the header
  std::cin.read(reinterpret_cast<char *>(&header), sizeof header);

  out << std::hex << std::showbase << header.riff_id << '\n'
      << header.riff_size << '\n'
      << header.wave_tag << '\n'
      << std::dec << header.data_size << " bytes of samples\n"
      << header.channels << " channel" << (header.channels > 1 ? "s" : "")
      << '\n';

  assert(header.sample_rate == 0xac44);

  // Read a batch of samples
  const size_t count{10000};

  using sample_t = int16_t;
  std::vector<sample_t> samples(count);
  std::cin.read(reinterpret_cast<char *>(samples.data()),
                samples.size() * sizeof(sample_t));

  out << samples.size() << " samples read from mic\n";

  const auto &[min, max] =
      std::minmax_element(std::cbegin(samples), std::cend(samples));
  std::cout << *min << "\tmin\n";
  std::cout << *max << "\tmax\n";

  // Dump to a CSV
  if (std::ofstream csv{"tmp/mic.csv"}; csv.good())
    for (const auto &s : samples)
      csv << std::hex << s << '\n';

  std::cout << out.str();
}

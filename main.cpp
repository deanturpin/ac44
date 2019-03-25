#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main() {

  struct wav_header {
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
  };

  assert(sizeof(wav_header) == 44);

  const std::string file{"../dft/wav/hobgoblin_didge_8000.wav"};

  if (std::ifstream in{file}; in.good()) {
    std::cout << file << " is good\n";

    wav_header h;
    in.read(reinterpret_cast<char *>(&h), sizeof h);

    std::cout << std::hex << std::showbase << h.riff_id << '\n'
              << h.riff_size << '\n'
              << h.wave_tag << '\n'
              << std::dec << h.data_size << " bytes\n"
              << h.channels << " channels\n";

    // Create a container for the samples
    std::vector<int16_t> s(h.data_size);
    in.read(reinterpret_cast<char *>(s.data()), s.size() * sizeof(int16_t));

    std::cout << s.size() << " samples read\n";

    // if (!s.empty())
    //   std::for_each(std::cbegin(s), std::next(std::cbegin(s), 30),
    //                 [](const auto &i) { std::cout << i << '\n'; });
  }
}

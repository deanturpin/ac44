#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

int main() {

  struct wav_header {
    uint32_t riff_id;
    uint32_t riff_size;
    uint32_t wave_tag;
    uint32_t format_id;
    uint32_t format_size;
    uint32_t format_tag : 16;
    uint32_t channels : 16;
    uint32_t sample_rate;
    uint32_t bytes_per_second;
    uint32_t block_align : 16;
    uint32_t bit_depth : 16;
    uint32_t data_id;
    uint32_t data_size;
  };

  assert(sizeof(wav_header) == 44);

  std::cout << "audio read\n";
  const std::string file{"../dft/wav/hobgoblin_didge_8000.wav"};

  if (std::ifstream in{file}; in.good()) {
    std::cout << file << " is good\n";

    wav_header h;
    in.read(reinterpret_cast<char *>(&h), sizeof h);

    std::cout << std::hex << std::showbase << h.riff_id << '\n'
              << h.riff_size << '\n'
              << h.wave_tag << '\n'
              << std::dec << h.data_size << " bytes\n";
  }
}

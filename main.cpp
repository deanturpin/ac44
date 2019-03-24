#include <fstream>
#include <iostream>
#include <string>

int main() {

  struct wav_header {
    using word = unsigned int;
    word riff_id;
    word riff_size;
    word wave_tag;
    word format_id;
    word format_size;
    word format_tag : 16;
    word channels : 16;
    word sample_rate;
    word bytes_per_second;
    word block_align : 16;
    word bit_depth : 16;
    word data_id;
    word data_size;
  };

  std::cout << "audio read\n";
  const std::string file{"../dft/wav/hobgoblin_didge_8000.wav"};

  if (std::ifstream in{file}; in.good())
    std::cout << file << " is good\n";

  std::ifstream in{"notafile"};
  std::cout << "file is " << (in.good() ? "good" : "bad") << '\n';
}

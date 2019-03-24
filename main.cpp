
#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::cout << "audio read\n";

  const std::string file{"../dft/wav/hobgoblin_didge_8000.wav"};

  if (std::ifstream in{file}; in.good())
    std::cout << file << " is good\n";
}

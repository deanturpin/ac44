#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

int main() {

  std::ostringstream out;

  // Read a batch of samples
  const size_t count{10000};

  using sample_t = int16_t;
  std::vector<sample_t> samples(count);
  std::cin.read(reinterpret_cast<char *>(samples.data()),
                samples.size() * sizeof(sample_t));

  out << samples.size() << " samples read from mic\n";

  const auto &[min, max] = std::minmax_element(std::cbegin(samples), std::cend(samples));
  std::cout << *min << "\tmin\n";
  std::cout << *max << "\tmax\n";

  // Dump to a CSV
  if (std::ofstream csv{"tmp/mic.csv"}; csv.good())
    for (const auto &s : samples)
      csv << s << '\n';

  std::cout << out.str();
}

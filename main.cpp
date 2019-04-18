#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// Structure of a WAV
struct ac44 {
  std::uint32_t riff_id{};
  std::uint32_t riff_size{};
  std::uint32_t wave_tag{};
  std::uint32_t format_id{};
  std::uint32_t format_size{};
  std::uint16_t format_tag{};
  std::uint16_t channels{};
  std::uint32_t sample_rate{};
  std::uint32_t bytes_per_second{};
  std::uint16_t block_align{};
  std::uint16_t bit_depth{};
  std::uint32_t data_id{};
  std::uint32_t data_size{};
};

// Read WAV header from a stream and return sample rate
ac44 get_meta(std::istream &in) {

  ac44 meta;
  assert(sizeof(meta) == 44);

  // Check file is good and read the meta data
  if (in.good())
    in.read(reinterpret_cast<char *>(&meta), sizeof meta);

  return meta;
}

// Initialse twiddle matrix
auto fourier_init(const size_t &bins) {

  using namespace std::complex_literals;

  // Declare container for twiddle matrix
  std::vector<std::complex<double>> twiddle;
  twiddle.reserve(bins * bins);

  // Populate twiddle matrix
  for (size_t k = 0; k < bins; ++k)
    for (size_t n = 0; n < bins; ++n)
      twiddle.push_back(exp(2i * M_PI * double(k) * double(n) / double(bins)));

  return twiddle;
}

using sample_t = int16_t;
std::vector<double> get_fourier(const std::vector<sample_t> &samples,
                                const size_t bins = 40) {

  // Initialise results container and reserve enough space for the bins
  std::vector<double> fourier;
  fourier.reserve(bins);

  // Initialise twiddle matrix on first call
  static const auto twiddle = fourier_init(bins);
  std::cout << twiddle.size() << " twiddle entries\n";

  // Calculate Fourier response
  for (size_t k = 0; k < bins; ++k) {

    // We want to calculate the sum of all responses
    std::complex<double> sum{};

    // Calculate response for each sample
    for (unsigned int n = 0; n < bins; ++n)
      sum += twiddle[(k * bins) + n] * std::complex<double>(samples.at(n), 0);

    // Store the absolute value of the complex sum
    fourier.push_back(abs(sum));
  }

  return fourier;
}

int main() {

  // Define source of audio
  auto &in = std::cin;

  // Get the meta data
  const auto &sample_rate = get_meta(in).sample_rate;

  // Prepare container to read a batch of samples
  std::vector<sample_t> samples(sample_rate);
  const size_t bytes_in_batch{samples.size() * sizeof(sample_t)};

  // Repeatedly read batches of samples and report stats until read fails
  while (in.read(reinterpret_cast<char *>(samples.data()), bytes_in_batch)) {

    const auto fourier = get_fourier(samples, 160);
    std::cout << samples.size() << " samples " << fourier.size()
              << " Fourier size\n";

    const double max_bin =
        *std::max_element(std::cbegin(fourier), std::cend(fourier));
    for (const auto &bin : fourier)
      std::cout << std::string(std::rint(350.0 * bin / max_bin), '-') << '\n';
  }
}

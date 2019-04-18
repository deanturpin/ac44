#include <algorithm>
#include <array>
#include <cassert>
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

const size_t fourier_bins = 1024;

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
auto fourier_init() {

  using namespace std::complex_literals;

  const double pi{
      3.1415926535897932384626433832795028841971693993751058209749445923078164062};
  const size_t bins{fourier_bins};

  // Declare container for twiddle matrix
  std::array<std::complex<double>, bins * bins / 2> twiddle;

  // Populate twiddle matrix
  for (size_t k = 0; k < bins / 2; ++k)
    for (size_t n = 0; n < bins; ++n)
      twiddle[n + (k * bins)] =
          exp(2i * pi * double(k) * double(n) / double(bins));

  return twiddle;
}

using sample_t = int16_t;
std::vector<double> get_fourier(const std::vector<sample_t> &samples) {

  // Initialise twiddle matrix on first call
  static const auto &twiddle = fourier_init();

  // Initialise results container and reserve enough space for the bins
  const size_t bins{fourier_bins};
  std::vector<double> fourier;
  fourier.reserve(bins);

  // Calculate Fourier response
  for (size_t k = 0; k < bins / 2; ++k) {

    // We want to calculate the sum of all responses
    std::complex<double> bin_sum{};

    // Calculate response for each sample
    const size_t k_times_bins = k * bins;
    for (unsigned int n = 0; n < bins; ++n)
      bin_sum +=
          twiddle[k_times_bins + n] * std::complex<double>(samples.at(n), 0);

    // bin_sum = std::accumulate(std::cbegin(samples), std::cend(samples),
    // std::complex<double>{},
    //                       [&, n = 0u](auto sum, const auto &s) mutable {
    //     return sum += twiddle[(k * bins) + n] * std::complex<double>(s, 0);

    //       ++n;
    //                       });

    // Store the absolute value of the complex sum
    fourier.push_back(abs(bin_sum));
  }

  return fourier;
}

// Create text representation of basic histogram
using iterator_t = std::vector<double>::const_iterator;
std::string dump_histogram(const iterator_t &begin, const iterator_t &end) {

  // Calculate max bin so we can scale the output
  const double max_bin{*std::max_element(begin, end)};

  // Max width of a bar
  const size_t max_width = 70;

  std::ostringstream out;
  out << '\n';
  std::for_each(begin, end, [&](const auto &bin) {
    out << std::string(1 + std::rint(max_width * bin / max_bin), '_')
        << "\033[36m_\033[0m\n";
  });

  return out.str();
}

// Create text representation of histogram with a log X axis
std::string dump_log_histogram(const iterator_t &begin, const iterator_t &end) {

  // Construct log histogram
  std::vector<double> log_histogram;
  log_histogram.reserve(std::distance(begin, end));

  std::for_each(begin, end, [&, n = 0](const auto &s) mutable {
    size_t index = std::rint(std::log2(n));

    // If it's a new bin create it
    if (index >= log_histogram.size())
      log_histogram.push_back(0.0);

    // Sum all the values that match a bin
    log_histogram.back() += s;

    ++n;
  });

  // Free up any unused memory
  log_histogram.shrink_to_fit();

  // Pass log histogram to basic histogram routine
  return dump_histogram(std::cbegin(log_histogram), std::cend(log_histogram));
}

int main() {

  // Define source of audio
  auto &in = std::cin;

  // Get the meta data
  const auto &sample_rate = get_meta(in).sample_rate;

  // Prepare container to read a batch of samples
  std::vector<sample_t> samples(sample_rate / 16);
  const size_t bytes_in_batch{samples.size() * sizeof(sample_t)};

  // Repeatedly read batches of samples and report stats until read fails
  while (in.read(reinterpret_cast<char *>(samples.data()), bytes_in_batch)) {

    // Get Fourier transform for this batch and dump it
    const auto &fourier = get_fourier(samples);
    std::cout << dump_log_histogram(std::cbegin(fourier), std::cend(fourier));
    // std::cout << dump_histogram(std::cbegin(fourier), std::cend(fourier));
  }
}

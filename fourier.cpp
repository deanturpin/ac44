#include "fourier.h"
#include <cassert>
#include <complex>

// Initialse twiddle matrix
std::vector<std::complex<double>> fourier_init() {

  // Declare container for twiddle matrix
  std::vector<std::complex<double>> twiddle;

  // Only need to calculate first half as the second is a mirror image
  twiddle.reserve(bins<size_t> * bins<size_t> / 2);

  const double pi{3.1415926535897932385};

  // Populate twiddle matrix
  using namespace std::complex_literals;
  for (size_t k = 0; k < bins<size_t> / 2; ++k)
    for (size_t n = 0; n < bins<size_t>; ++n)
      twiddle.push_back(std::exp(2i * pi * static_cast<double>(k) *
                                 static_cast<double>(n) / bins<double>));

  return twiddle;
}

// Calculate Fourier response for a container of samples
std::vector<double> get_fourier(const std::vector<int16_t> &samples) {

  assert(samples.size() >= bins<size_t> &&
         "Fourier bins greater than samples size");

  // Initialise twiddle matrix on first call
  static const auto &twiddle = fourier_init();

  // Initialise results container and reserve enough space for the bins
  std::vector<double> fourier;
  fourier.reserve(bins<size_t>);

  // Calculate Fourier response
  for (size_t k = 0; k < bins<size_t> / 2; ++k) {

    // We want to calculate the sum of all responses
    std::complex<double> sum{};

    // Calculate response for each sample
    for (size_t n = 0; n < bins<size_t>; ++n)
      sum += twiddle[(k * bins<size_t>)+n] *
             std::complex<double>(samples.at(n), 0);

    // Store the absolute value of the complex sum
    fourier.push_back(std::abs(sum));
  }

  return fourier;
}

#ifndef FOURIER_H
#define FOURIER_H

#include <cstdint>
#include <vector>

const size_t fourier_bins = 4 * 1024;

using sample_t = int16_t;
std::vector<double> get_fourier(const std::vector<sample_t> &);

#endif

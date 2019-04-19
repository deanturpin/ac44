#ifndef FOURIER_H
#define FOURIER_H

#include <cstdint>
#include <vector>

using sample_t = int16_t;
std::vector<double> get_fourier(const std::vector<sample_t> &);

#endif

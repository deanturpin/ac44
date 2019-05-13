#ifndef FOURIER_H
#define FOURIER_H

#include <cstdint>
#include <vector>

template <class T> constexpr T bins = 8 * 1024;
std::vector<double> get_fourier(const std::vector<int16_t> &);

#endif

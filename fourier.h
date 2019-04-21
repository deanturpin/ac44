#ifndef FOURIER_H
#define FOURIER_H

#include <cstdint>
#include <vector>

template <class T> constexpr T bins = 4 * 1024;
template <class T> constexpr T pi   = T(3.1415926535897932385L);

std::vector<double> get_fourier(const std::vector<int16_t> &);

#endif

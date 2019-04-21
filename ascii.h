#ifndef ASCII_H
#define ASCII_H

#include <string>
#include <vector>

using iterator_t = std::vector<double>::const_iterator;
std::string dump_histogram(const iterator_t &, const iterator_t &);
std::string dump_log_histogram(const iterator_t &, const iterator_t &);
std::string dump_aerial_histogram(const iterator_t &, const iterator_t &);

#endif

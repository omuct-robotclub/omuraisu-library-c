#ifndef OMURAISU_CPP_COBS_COBS_HPP_
#define OMURAISU_CPP_COBS_COBS_HPP_

#include <vector>

#include "cobs/cobs.h"

namespace omuraisu {
namespace cobs {
std::vector<uint8_t> encode(const std::vector<uint8_t>& data);
std::vector<uint8_t> decode(const std::vector<uint8_t>& data);
}  // namespace cobs
}  // namespace omuraisu

#endif  // OMURAISU_CPP_COBS_COBS_HPP_
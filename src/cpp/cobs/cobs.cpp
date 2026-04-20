#include "cobs/cobs.hpp"

#include <vector>

namespace omuraisu {
namespace cobs {
std::vector<uint8_t> encode(const std::vector<uint8_t>& data) {
  if (data.empty()) {
    return {0x01, 0x00};
  }
  size_t alloc_length = data.size() + (data.size() / 254) + 2;
  std::vector<uint8_t> encoded(alloc_length, 0);
  if (!om_cobs_encode(data.data(), data.size(), encoded.data(),
                      &alloc_length)) {
    return {};
  }
  if (alloc_length == 0 || encoded[alloc_length - 1] != 0x00) {
    return {};
  }
  encoded.resize(alloc_length);
  return encoded;
}
std::vector<uint8_t> decode(const std::vector<uint8_t>& data) {
  size_t alloc_length = data.size();
  std::vector<uint8_t> decoded(alloc_length, 0);
  if (!om_cobs_decode(data.data(), data.size(), decoded.data(),
                      &alloc_length)) {
    return {};
  }
  if (alloc_length == 0) {
    return {};
  }
  decoded.resize(alloc_length);
  return decoded;
}
}  // namespace cobs
}  // namespace omuraisu
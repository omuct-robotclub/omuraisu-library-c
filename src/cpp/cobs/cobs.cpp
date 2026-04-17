#include "cobs/cobs.hpp"

#include <vector>

namespace omuraisu {
namespace cobs {
std::vector<uint8_t> encode(const std::vector<uint8_t>& data) {
  if (data.empty()) {
    return {0x01, 0x00};
  }
  size_t alloc_length = data.size() + (data.size() / 254) + 2;
  uint8_t raw_encoded[alloc_length] = {0};
  if (!om_cobs_encode(data.data(), data.size(), raw_encoded, &alloc_length)) {
    return {};
  }
  if (alloc_length == 0 || raw_encoded[alloc_length - 1] != 0x00) {
    return {};
  }
  return std::vector<uint8_t>(raw_encoded, raw_encoded + alloc_length);
}
std::vector<uint8_t> decode(const std::vector<uint8_t>& data) {
  size_t alloc_length = data.size();
  uint8_t raw_decoded[alloc_length] = {0};
  if (!om_cobs_decode(data.data(), data.size(), raw_decoded, &alloc_length)) {
    return {};
  }
  if (alloc_length == 0) {
    return {};
  }
  return std::vector<uint8_t>(raw_decoded, raw_decoded + alloc_length);
}
}  // namespace cobs
}  // namespace omuraisu
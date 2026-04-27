#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "cobs/cobs.hpp"

namespace {

bool ExpectTrue(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << message << std::endl;
    return false;
  }
  return true;
}

bool TestRoundTripBasic() {
  const std::vector<uint8_t> original = {0x00, 0x11, 0x00, 0x22, 0x33, 0x00};

  const std::vector<uint8_t> encoded = omuraisu::cobs::encode(original);
  if (!ExpectTrue(!encoded.empty(), "basic: encode returned empty")) {
    return false;
  }
  if (!ExpectTrue(encoded.back() == 0x00,
                  "basic: encoded payload delimiter is invalid")) {
    return false;
  }

  const std::vector<uint8_t> decoded = omuraisu::cobs::decode(encoded);
  return ExpectTrue(decoded == original,
                    "basic: decoded payload does not match original");
}

bool TestDecodeInvalidInputs() {
  // Leading code byte cannot be delimiter value.
  const std::vector<uint8_t> invalid_code_zero = {0x00, 0x00};
  const std::vector<uint8_t> decoded_code_zero =
      omuraisu::cobs::decode(invalid_code_zero);
  if (!ExpectTrue(decoded_code_zero.empty(),
                  "invalid: code-zero frame should fail decode")) {
    return false;
  }

  // Block size overflows trimmed payload.
  const std::vector<uint8_t> invalid_block = {0x05, 0x11, 0x22, 0x00};
  const std::vector<uint8_t> decoded_block =
      omuraisu::cobs::decode(invalid_block);
  if (!ExpectTrue(decoded_block.empty(),
                  "invalid: oversized block should fail decode")) {
    return false;
  }

  // Missing trailing delimiter in encoded stream.
  const std::vector<uint8_t> invalid_no_delimiter = {0x03, 0x11, 0x22};
  const std::vector<uint8_t> decoded_no_delimiter =
      omuraisu::cobs::decode(invalid_no_delimiter);
  return ExpectTrue(decoded_no_delimiter.empty(),
                    "invalid: missing delimiter should fail decode");
}

bool TestRoundTripLargePayload() {
  std::vector<uint8_t> original;
  original.reserve(4096);
  for (size_t i = 0; i < 4096; ++i) {
    if (i % 97 == 0) {
      original.push_back(0x00);
    } else {
      original.push_back(static_cast<uint8_t>((i * 37) & 0xFF));
    }
  }

  const std::vector<uint8_t> encoded = omuraisu::cobs::encode(original);
  if (!ExpectTrue(!encoded.empty(), "large: encode returned empty")) {
    return false;
  }
  if (!ExpectTrue(encoded.back() == 0x00,
                  "large: encoded payload delimiter is invalid")) {
    return false;
  }

  const std::vector<uint8_t> decoded = omuraisu::cobs::decode(encoded);
  return ExpectTrue(decoded == original,
                    "large: decoded payload does not match original");
}

bool TestRoundTripLongNonZeroRun() {
  std::vector<uint8_t> original(1024, 0x7E);

  const std::vector<uint8_t> encoded = omuraisu::cobs::encode(original);
  if (!ExpectTrue(!encoded.empty(), "long-run: encode returned empty")) {
    return false;
  }

  const std::vector<uint8_t> decoded = omuraisu::cobs::decode(encoded);
  return ExpectTrue(decoded == original,
                    "long-run: decoded payload does not match original");
}

bool TestBoundaryLengths() {
  const std::vector<size_t> lengths = {0, 1, 253, 254, 255, 508, 509};

  for (size_t length : lengths) {
    std::vector<uint8_t> original(length, 0xAB);

    const std::vector<uint8_t> encoded = omuraisu::cobs::encode(original);
    if (!ExpectTrue(!encoded.empty(),
                    "boundary(non-zero): encode returned empty at length " +
                        std::to_string(length))) {
      return false;
    }
    if (!ExpectTrue(encoded.back() == 0x00,
                    "boundary(non-zero): invalid delimiter at length " +
                        std::to_string(length))) {
      return false;
    }

    const std::vector<uint8_t> decoded = omuraisu::cobs::decode(encoded);
    if (!ExpectTrue(decoded == original,
                    "boundary(non-zero): round-trip failed at length " +
                        std::to_string(length))) {
      return false;
    }
  }

  // Place zero at boundary-adjacent positions to exercise block splitting.
  std::vector<uint8_t> with_zero_at_253(509, 0xCD);
  with_zero_at_253[253] = 0x00;
  std::vector<uint8_t> with_zero_at_254(509, 0xEF);
  with_zero_at_254[254] = 0x00;

  const std::vector<std::vector<uint8_t>> zero_boundary_cases = {
      with_zero_at_253, with_zero_at_254};

  for (size_t i = 0; i < zero_boundary_cases.size(); ++i) {
    const std::vector<uint8_t>& original = zero_boundary_cases[i];
    const std::vector<uint8_t> encoded = omuraisu::cobs::encode(original);
    if (!ExpectTrue(!encoded.empty(),
                    "boundary(zero): encode returned empty at case " +
                        std::to_string(i))) {
      return false;
    }
    if (!ExpectTrue(
            encoded.back() == 0x00,
            "boundary(zero): invalid delimiter at case " + std::to_string(i))) {
      return false;
    }

    const std::vector<uint8_t> decoded = omuraisu::cobs::decode(encoded);
    if (!ExpectTrue(
            decoded == original,
            "boundary(zero): round-trip failed at case " + std::to_string(i))) {
      return false;
    }
  }

  return true;
}

}  // namespace

int main() {
  bool ok = true;

  ok = TestRoundTripBasic() && ok;
  ok = TestDecodeInvalidInputs() && ok;
  ok = TestRoundTripLargePayload() && ok;
  ok = TestRoundTripLongNonZeroRun() && ok;
  ok = TestBoundaryLengths() && ok;

  if (!ok) {
    std::cerr << "cobs_cpp_test failed" << std::endl;
    return 1;
  }

  std::cout << "cobs_cpp_test passed" << std::endl;
  return 0;
}

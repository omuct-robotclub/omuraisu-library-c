#include "cobs.h"

bool om_cobs_encode(const uint8_t* data, size_t length, uint8_t* encoded,
                    size_t* encoded_length) {
  if (data == NULL || encoded == NULL || encoded_length == NULL) {
    return false;
  }

  const size_t output_capacity = *encoded_length;
  const size_t max_encoded_length = length + (length / 254) + 2;
  if (output_capacity < max_encoded_length) {
    return false;
  }

  size_t code_index = 0;
  size_t write_index = 1;
  uint8_t code = 1;

  for (size_t i = 0; i < length; ++i) {
    if (data[i] == 0x00) {
      encoded[code_index] = code;
      code_index = write_index;
      ++write_index;
      code = 1;
    } else {
      encoded[write_index] = data[i];
      ++write_index;
      ++code;

      if (code == 0xFF) {
        encoded[code_index] = code;
        code_index = write_index;
        ++write_index;
        code = 1;
      }
    }
  }

  encoded[code_index] = code;
  encoded[write_index] = 0x00;
  *encoded_length = write_index + 1;
  return true;
}

bool om_cobs_decode(const uint8_t* encoded, size_t encoded_length,
                    uint8_t* decoded, size_t* decoded_length) {
  if (encoded == NULL || decoded == NULL || decoded_length == NULL) {
    return false;
  }

  if (encoded_length == 0) {
    return false;
  }

  const size_t output_capacity = *decoded_length;
  const size_t trimmed_length = encoded_length - 1;
  const uint8_t delimiter = encoded[trimmed_length];

  size_t read_index = 0;
  size_t write_index = 0;
  while (read_index < trimmed_length) {
    const uint8_t code = encoded[read_index];
    if (code == delimiter) {
      // Invalid COBS encoded data
      return false;
    }
    ++read_index;

    const size_t block_size = (size_t)code - 1;
    if (read_index + block_size > trimmed_length) {
      // Invalid COBS encoded data
      return false;
    }

    if (write_index + block_size > output_capacity) {
      return false;
    }

    for (size_t j = 0; j < block_size; ++j) {
      decoded[write_index] = encoded[read_index + j];
      ++write_index;
    }
    read_index += block_size;

    if (code < 0xFF && read_index < trimmed_length) {
      if (write_index >= output_capacity) {
        return false;
      }
      decoded[write_index] = delimiter;
      ++write_index;
    }
  }

  *decoded_length = write_index;
  return true;
}

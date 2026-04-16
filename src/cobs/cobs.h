#ifndef OM_COBS_H_
#define OM_COBS_H_
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool om_cobs_encode(const uint8_t* data, size_t length, uint8_t* encoded,
                    size_t* encoded_length);
bool om_cobs_decode(const uint8_t* encoded, size_t encoded_length,
                    uint8_t* decoded, size_t* decoded_length);

#endif  // OM_COBS_H_
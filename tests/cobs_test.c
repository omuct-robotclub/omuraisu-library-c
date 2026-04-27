#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cobs/cobs.h"

#define TEST_PASS() printf("✓ PASS\n")
#define TEST_FAIL() printf("✗ FAIL\n")
#define TEST_START(name) printf("\n[Test] %s ... ", name)

static int test_count = 0;
static int pass_count = 0;

void assert_true(bool condition, const char* message) {
  if (!condition) {
    printf("(assertion failed: %s) ", message);
    test_count++;
  }
}

void assert_equal_size(size_t actual, size_t expected, const char* message) {
  if (actual != expected) {
    printf("(expected %zu, got %zu: %s) ", expected, actual, message);
  }
}

void assert_equal_bytes(const uint8_t* actual, const uint8_t* expected,
                        size_t length, const char* message) {
  if (memcmp(actual, expected, length) != 0) {
    printf("(bytes mismatch: %s) ", message);
  }
}

// Test 1: Basic encode
void test_cobs_encode_basic(void) {
  TEST_START("COBS encode basic");

  uint8_t data[] = {0x11, 0x22, 0x00, 0x33};
  uint8_t encoded[16] = {0};
  size_t encoded_length = 16;

  bool result = om_cobs_encode(data, sizeof(data), encoded, &encoded_length);

  assert_true(result, "encode should succeed");
  // Expected: 0x02 0x11 0x22 0x02 0x33 0x00
  // (0x02 means "2 bytes until next zero", 0x33 is the data, 0x00 is delimiter)
  uint8_t expected[] = {0x03, 0x11, 0x22, 0x02, 0x33, 0x00};
  assert_equal_size(encoded_length, sizeof(expected), "encoded length");
  assert_equal_bytes(encoded, expected, encoded_length, "encoded data");

  if (result && encoded_length == sizeof(expected) && 
      memcmp(encoded, expected, encoded_length) == 0) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 2: Basic decode
void test_cobs_decode_basic(void) {
  TEST_START("COBS decode basic");

  uint8_t encoded[] = {0x03, 0x11, 0x22, 0x02, 0x33, 0x00};
  uint8_t decoded[16] = {0};
  size_t decoded_length = 16;

  bool result = om_cobs_decode(encoded, sizeof(encoded), decoded, &decoded_length);

  assert_true(result, "decode should succeed");
  uint8_t expected[] = {0x11, 0x22, 0x00, 0x33};
  assert_equal_size(decoded_length, sizeof(expected), "decoded length");
  assert_equal_bytes(decoded, expected, decoded_length, "decoded data");

  if (result && decoded_length == sizeof(expected) &&
      memcmp(decoded, expected, decoded_length) == 0) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 3: Encode empty data
void test_cobs_encode_empty(void) {
  TEST_START("COBS encode empty data");

  uint8_t data[] = {};
  uint8_t encoded[16] = {0};
  size_t encoded_length = 16;

  bool result = om_cobs_encode(data, 0, encoded, &encoded_length);

  assert_true(result, "encode should succeed");
  // Empty data should produce: 0x01 0x00
  uint8_t expected[] = {0x01, 0x00};
  assert_equal_size(encoded_length, sizeof(expected), "encoded length");
  assert_equal_bytes(encoded, expected, encoded_length, "encoded data");

  if (result && encoded_length == sizeof(expected) &&
      memcmp(encoded, expected, encoded_length) == 0) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 4: Data without null bytes
void test_cobs_encode_no_null(void) {
  TEST_START("COBS encode data without null");

  uint8_t data[] = {0x11, 0x22, 0x33};
  uint8_t encoded[16] = {0};
  size_t encoded_length = 16;

  bool result = om_cobs_encode(data, sizeof(data), encoded, &encoded_length);

  assert_true(result, "encode should succeed");
  // Expected: 0x04 0x11 0x22 0x33 0x00
  uint8_t expected[] = {0x04, 0x11, 0x22, 0x33, 0x00};
  assert_equal_size(encoded_length, sizeof(expected), "encoded length");
  assert_equal_bytes(encoded, expected, encoded_length, "encoded data");

  if (result && encoded_length == sizeof(expected) &&
      memcmp(encoded, expected, encoded_length) == 0) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 5: Multiple consecutive null bytes
void test_cobs_encode_multiple_nulls(void) {
  TEST_START("COBS encode multiple null bytes");

  uint8_t data[] = {0x00, 0x00, 0x11};
  uint8_t encoded[16] = {0};
  size_t encoded_length = 16;

  bool result = om_cobs_encode(data, sizeof(data), encoded, &encoded_length);

  assert_true(result, "encode should succeed");
  // Expected: 0x01 0x01 0x02 0x11 0x00
  uint8_t expected[] = {0x01, 0x01, 0x02, 0x11, 0x00};
  assert_equal_size(encoded_length, sizeof(expected), "encoded length");
  assert_equal_bytes(encoded, expected, encoded_length, "encoded data");

  if (result && encoded_length == sizeof(expected) &&
      memcmp(encoded, expected, encoded_length) == 0) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 6: Decode empty data
void test_cobs_decode_empty(void) {
  TEST_START("COBS decode empty data");

  uint8_t encoded[] = {0x01, 0x00};
  uint8_t decoded[16] = {0};
  size_t decoded_length = 16;

  bool result = om_cobs_decode(encoded, sizeof(encoded), decoded, &decoded_length);

  assert_true(result, "decode should succeed");
  assert_equal_size(decoded_length, 0, "decoded length");

  if (result && decoded_length == 0) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 7: NULL pointer handling
void test_cobs_null_pointers(void) {
  TEST_START("COBS null pointer handling");

  uint8_t data[] = {0x11, 0x22};
  uint8_t buffer[16] = {0};
  size_t length = 16;

  // Test encode with NULL data
  bool result1 = om_cobs_encode(NULL, sizeof(data), buffer, &length);
  assert_true(!result1, "encode with NULL data should fail");

  // Test encode with NULL encoded
  result1 = om_cobs_encode(data, sizeof(data), NULL, &length);
  assert_true(!result1, "encode with NULL encoded should fail");

  // Test encode with NULL length
  result1 = om_cobs_encode(data, sizeof(data), buffer, NULL);
  assert_true(!result1, "encode with NULL length should fail");

  // Test decode with NULL encoded
  bool result2 = om_cobs_decode(NULL, sizeof(data), buffer, &length);
  assert_true(!result2, "decode with NULL encoded should fail");

  // Test decode with NULL decoded
  uint8_t encoded[] = {0x02, 0x11, 0x00};
  result2 = om_cobs_decode(encoded, sizeof(encoded), NULL, &length);
  assert_true(!result2, "decode with NULL decoded should fail");

  // Test decode with NULL length
  result2 = om_cobs_decode(encoded, sizeof(encoded), buffer, NULL);
  assert_true(!result2, "decode with NULL length should fail");

  if (!om_cobs_encode(NULL, sizeof(data), buffer, &length) &&
      !om_cobs_encode(data, sizeof(data), NULL, &length) &&
      !om_cobs_decode(NULL, sizeof(data), buffer, &length)) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 8: Buffer overflow protection (encode)
void test_cobs_encode_buffer_overflow(void) {
  TEST_START("COBS encode buffer overflow protection");

  uint8_t data[] = {0x11, 0x22, 0x33, 0x44, 0x55};
  uint8_t encoded[2] = {0};  // Way too small
  size_t encoded_length = sizeof(encoded);

  bool result = om_cobs_encode(data, sizeof(data), encoded, &encoded_length);

  assert_true(!result, "encode should fail with small buffer");

  if (!result) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 9: Buffer overflow protection (decode)
void test_cobs_decode_buffer_overflow(void) {
  TEST_START("COBS decode buffer overflow protection");

  uint8_t encoded[] = {0x04, 0x11, 0x22, 0x33, 0x00};
  uint8_t decoded[2] = {0};  // Too small for decoded data
  size_t decoded_length = sizeof(decoded);

  bool result = om_cobs_decode(encoded, sizeof(encoded), decoded, &decoded_length);

  assert_true(!result, "decode should fail with small buffer");

  if (!result) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

// Test 10: Round-trip test
void test_cobs_roundtrip(void) {
  TEST_START("COBS round-trip (encode then decode)");

  uint8_t original[] = {0x00, 0x11, 0x00, 0x22, 0x33, 0x00};
  uint8_t encoded[64] = {0};
  uint8_t decoded[64] = {0};
  
  size_t encoded_length = sizeof(encoded);
  bool encode_result = om_cobs_encode(original, sizeof(original), 
                                       encoded, &encoded_length);
  
  size_t decoded_length = sizeof(decoded);
  bool decode_result = om_cobs_decode(encoded, encoded_length, 
                                       decoded, &decoded_length);

  assert_true(encode_result, "encode should succeed");
  assert_true(decode_result, "decode should succeed");
  assert_equal_size(decoded_length, sizeof(original), "decoded length matches original");
  assert_equal_bytes(decoded, original, decoded_length, "decoded data matches original");

  if (encode_result && decode_result && decoded_length == sizeof(original) &&
      memcmp(decoded, original, decoded_length) == 0) {
    TEST_PASS();
    pass_count++;
  } else {
    TEST_FAIL();
  }
  test_count++;
}

int main(void) {
  printf("========================================\n");
  printf("COBS Library Test Suite\n");
  printf("========================================\n");

  test_cobs_encode_basic();
  test_cobs_decode_basic();
  test_cobs_encode_empty();
  test_cobs_encode_no_null();
  test_cobs_encode_multiple_nulls();
  test_cobs_decode_empty();
  test_cobs_null_pointers();
  test_cobs_encode_buffer_overflow();
  test_cobs_decode_buffer_overflow();
  test_cobs_roundtrip();

  printf("\n========================================\n");
  printf("Results: %d/%d tests passed\n", pass_count, test_count);
  printf("========================================\n");

  return (pass_count == test_count) ? 0 : 1;
}

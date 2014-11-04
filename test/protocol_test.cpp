#include "../protocol.hpp"

#include <gtest/gtest.h>
#include <algorithm>

TEST(ProtocolTest, decode_uint64) {
  using websocket::internal::decode_uint64;

  std::vector<uint64_t> decoded = {0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00};
  EXPECT_EQ(0, decode_uint64(decoded.begin()));

  decoded = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
  EXPECT_EQ(1, decode_uint64(decoded.begin()));
  
  decoded = {0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  EXPECT_EQ(0x1f00000000000000, decode_uint64(decoded.begin()));
}

TEST(ProtocolTest, encode) {
  using websocket::internal::decode_uint64;
  using websocket::internal::Protocol;

  std::vector<uint8_t> result(0xfffff);
  std::string message;
  uint8_t masking_key[] = {0x12, 0x34, 0x56, 0x67};

  Protocol protocol;
  for (int i = 0; i < 4; i++) protocol.masking_key[i] = masking_key[i];
  protocol.FIN = 0;
  protocol.RSV1 = 1;
  protocol.RSV2 = 0;
  protocol.RSV3 = 1;
  protocol.mask = 0;
  protocol.opcode = Protocol::opcode_type::TEXT_FRAME;
  message = "hoge";

  auto end = protocol.encode(message.begin(), message.end(), result.begin());
  EXPECT_EQ(6, end - result.begin());

  EXPECT_EQ(0x51, result[0]);  // 01010001
  EXPECT_EQ(4, result[1]);
  EXPECT_EQ('h', result[2]);
  EXPECT_EQ('o', result[3]);
  EXPECT_EQ('g', result[4]);
  EXPECT_EQ('e', result[5]);

  protocol.mask = 1;
  message = "hello";
  end = protocol.encode(message.begin(), message.end(), result.begin());

  EXPECT_EQ(0x85, result[1]);
  EXPECT_EQ(masking_key[0], result[2]);
  EXPECT_EQ(masking_key[1], result[3]);
  EXPECT_EQ(masking_key[2], result[4]);
  EXPECT_EQ(masking_key[3], result[5]);
  EXPECT_EQ('h' ^ masking_key[0], result[6]);
  EXPECT_EQ('e' ^ masking_key[1], result[7]);
  EXPECT_EQ('l' ^ masking_key[2], result[8]);
  EXPECT_EQ('l' ^ masking_key[3], result[9]);
  EXPECT_EQ('o' ^ masking_key[0], result[10]);

  protocol.mask = 1;
  message.assign(0xfff, 'a');
  end = protocol.encode(message.begin(), message.end(), result.begin());

  EXPECT_EQ(0xfff, message.size());
  EXPECT_EQ(0xfff + 8, end - result.begin());
  EXPECT_EQ(126, result[1] & 0x7f);  // check 7bit
  EXPECT_EQ(0xfff, static_cast<uint32_t>(result[2]) << 8 | result[3]);

  message.assign(0x10000, 'a');
  protocol.mask = 0;
  end = protocol.encode(message.begin(), message.end(), result.begin());
  EXPECT_EQ(127, result[1] & 0x7f);  // check 7bit
  EXPECT_EQ(0x10000, decode_uint64(result.begin() + 2));
  EXPECT_EQ('a', result[0xffff]);
}

TEST(ProtocolTest, decode) {
  using websocket::internal::Protocol;

  std::vector<uint8_t> encoded(0xfffff);

  Protocol protocol;
  Protocol response;
  std::string message;

  protocol.FIN = 0;
  protocol.RSV1 = 1;
  protocol.RSV2 = 0;
  protocol.RSV3 = 1;
  protocol.mask = 0;
  protocol.opcode = Protocol::opcode_type::TEXT_FRAME;

  message = "hello";
  protocol.encode(message.begin(), message.end(), encoded.begin());

  response.decode_header(encoded.begin());
  EXPECT_EQ(0, response.FIN);
  EXPECT_EQ(1, response.RSV1);
  EXPECT_EQ(0, response.RSV2);
  EXPECT_EQ(1, response.RSV3);
  EXPECT_EQ(5, response.payload_len);

  message.assign(125, 'a');
  protocol.encode(message.begin(), message.end(), encoded.begin());
  response.decode_header(encoded.begin());
  EXPECT_EQ(125, response.payload_len);

  message.assign(1000, 'a');
  protocol.encode(message.begin(), message.end(), encoded.begin());
  response.decode_header(encoded.begin());
  EXPECT_EQ(126, response.payload_len);

  message.assign(0x10000, 'a');
  protocol.encode(message.begin(), message.end(), encoded.begin());
  response.decode_header(encoded.begin());
  EXPECT_EQ(127, response.payload_len);
}

TEST(ProtocolTest, decode_expandables) {
  using websocket::internal::Protocol;

  std::vector<uint8_t> encoded(0xfffff);

  Protocol protocol;
  Protocol response;
  std::string message;

  protocol.mask = 0;
  message = "hello";
  protocol.encode(message.begin(), message.end(), encoded.begin());
  response.decode_expandables(
      response.decode_header(encoded.begin()));
  EXPECT_EQ(5, response.length);

  protocol.mask = 1;
  protocol.masking_key[0] = 0x1a;
  protocol.masking_key[1] = 0x2b;
  protocol.masking_key[2] = 0x3c;
  protocol.masking_key[3] = 0x4d;
  protocol.encode(message.begin(), message.end(), encoded.begin());
  response.decode_expandables(
      response.decode_header(encoded.begin()));
  EXPECT_EQ(0x1a, response.masking_key[0]);
  EXPECT_EQ(0x2b, response.masking_key[1]);
  EXPECT_EQ(0x3c, response.masking_key[2]);
  EXPECT_EQ(0x4d, response.masking_key[3]);

  message.assign(1000, 'a');
  protocol.encode(message.begin(), message.end(), encoded.begin());
  response.decode_expandables(
      response.decode_header(encoded.begin()));
  EXPECT_EQ(1000, response.length);

  message.assign(0x10000, 'a');
  protocol.encode(message.begin(), message.end(), encoded.begin());
  response.decode_expandables(
      response.decode_header(encoded.begin()));
  EXPECT_EQ(0x10000, response.length);
}

TEST(ProtocolTest, decode_payload) {
  using websocket::internal::Protocol;

  std::vector<uint8_t> encoded(0xffffff);

  Protocol protocol;
  Protocol response;
  std::string message;
  std::string result;

  // TODO: something is wrong when length > 0xffff 
  message.assign(0xffff, 'a');
  protocol.encode(message.begin(), message.end(), encoded.begin());

  auto input = response.decode_header(encoded.begin()); 
  input = response.decode_expandables(input);
  result.resize(response.length);

  response.decode_payload(input, result.begin());

  EXPECT_EQ(message, result);
}


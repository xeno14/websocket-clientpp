#include "websocket-clientpp.hpp"

#include <gtest/gtest.h>
#include <memory>


TEST(WebSocketClientppTest, parse_url) {
  using websocket::internal::parse_url;

  std::string hostname;
  std::string path;
  int port;

  EXPECT_TRUE(parse_url("ws://hogehoge.com:1234/aaa/bbbbbb/cccccccc", &hostname,
                        &path, &port));
  EXPECT_EQ("hogehoge.com", hostname);
  EXPECT_EQ("aaa/bbbbbb/cccccccc", path);
  EXPECT_EQ(1234, port);

  EXPECT_TRUE(parse_url("ws://hogehoge.com/aaa/bbbbbb/cccccccc", &hostname,
                        &path, &port));
  EXPECT_EQ("hogehoge.com", hostname);
  EXPECT_EQ(80, port);

  EXPECT_TRUE(parse_url("ws://hogehoge.com", &hostname, &path, &port));
  EXPECT_EQ("hogehoge.com", hostname);
  EXPECT_EQ("", path);
  EXPECT_EQ(80, port);

  EXPECT_TRUE(parse_url("ws://hogehoge.com/", &hostname, &path, &port));
  EXPECT_EQ("hogehoge.com", hostname);
  EXPECT_EQ("", path);
  EXPECT_EQ(80, port);
}

TEST(WebSocketClientppTest, connect_form_hostname) {
  using websocket::internal::connect_form_hostname;

  int sockfd;

  sockfd = connect_form_hostname("echo.websocket.org", 80);
  EXPECT_NE(-1, sockfd);
  ::close(sockfd);

  // sockfd = connect_form_hostname("127.0.0.1", 10000);
  // EXPECT_NE(-1, sockfd);
  // ::close(sockfd);

  sockfd = connect_form_hostname("thishostdoesnotexists", 80);
  EXPECT_EQ(-2, sockfd);
  ::close(sockfd);
}

TEST(WebSocketClientppTest, create_connection) {
  std::unique_ptr<websocket::WebSocket> ws(
      websocket::create_connection("ws://echo.websocket.org"));
  EXPECT_NE(nullptr, ws);
}

TEST(WebSocketClientppTest, send) {
  std::unique_ptr<websocket::WebSocket> ws(
      websocket::create_connection("ws://echo.websocket.org"));
  ASSERT_NE(nullptr, ws);

  uint8_t data[1024];
  websocket::internal::send(ws->sockfd(), data, "hello");
}

TEST(WebSocketClientppTest, send_recv) {
  std::unique_ptr<websocket::WebSocket> ws(
      websocket::create_connection("ws://echo.websocket.org"));
  ws->send("hello");
  EXPECT_EQ("hello", ws->recv());
}

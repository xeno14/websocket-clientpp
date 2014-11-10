#include "../websocket-clientpp.hpp"

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

TEST(WebSocketClientppTest, connect_from_hostname) {
  using websocket::internal::connect_from_hostname;

  int sockfd;

  sockfd = connect_from_hostname("echo.websocket.org", 80);
  EXPECT_NE(-1, sockfd);
  ::close(sockfd);

  // sockfd = connect_from_hostname("127.0.0.1", 10000);
  // EXPECT_NE(-1, sockfd);
  // ::close(sockfd);

  sockfd = connect_from_hostname("thishostdoesnotexists", 80);
  EXPECT_EQ(-2, sockfd);
  ::close(sockfd);
}

TEST(WebSocketClientppTest, create_connection) {
  std::unique_ptr<websocket::WebSocket> ws(
      websocket::create_connection("ws://echo.websocket.org"));
  EXPECT_NE(nullptr, ws);

  std::unique_ptr<websocket::WebSocket> ws_invalid(
      websocket::create_connection("ws://echo.websocketaaaaaaaaa.org"));
  EXPECT_EQ(nullptr, ws_invalid);
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

TEST(WebSocketClientppTest, send_timeout) {
  std::unique_ptr<websocket::WebSocket> ws(
      websocket::create_connection("ws://echo.websocket.org"));
  ASSERT_NE(nullptr, ws);

  ws->send("hello world");
  EXPECT_EQ("hello world", ws->recv(1000));  // long enough

  ws->send("hello world");
  EXPECT_EQ("", ws->recv(1));  // too short
}

TEST(WebSocketClientppTest, resize) {
  std::unique_ptr<websocket::WebSocket> ws(
      websocket::create_connection("ws://echo.websocket.org"));
  ASSERT_NE(nullptr, ws);

  std::string message(10000, 'a');
  ws->send(message);
  ws->recv();
  EXPECT_EQ(10014, ws->send_buf_size());
  EXPECT_EQ(10014, ws->recv_buf_size());
}

TEST(WebSocketClientppTest, callbacks) {
  using websocket::WebSocket;
  websocket::WebSocketApp app("ws://echo.websocket.org");
  app.set_timeout(1000);
  
  bool open_called = false;

  app.on_open([&open_called](WebSocket* ws) {
    open_called = true;
    ws->send("yeah");
  });
  app.on_message([](WebSocket* ws, std::string msg) {
    EXPECT_EQ("yeah", msg);
    ws->close();
  });

  app.run_forever();
}

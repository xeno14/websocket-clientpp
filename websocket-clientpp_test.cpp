#include "websocket-clientpp.hpp"

#include <gtest/gtest.h>


TEST(WebSocketClientppTest, connect_form_hostname) {
  using websocket::internal::connect_form_hostname;

  int sockfd;
  
  sockfd = connect_form_hostname("echo.websocket.org", 80);
  EXPECT_NE(-1, sockfd);
  ::close(sockfd);

  int sockfd = connect_form_hostname("echo.websocket.org", 100);
  EXPECT_EQ(-1, sockfd);
  ::close(sockfd);

  int sockfd = connect_form_hostname("echo.websocket.org", 100);
  EXPECT_EQ(-1, sockfd);
  ::close(sockfd);
}

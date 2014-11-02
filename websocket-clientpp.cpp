#include "websocket-clientpp.hpp"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/tcp.h>

namespace {

constexpr int INVALID_SOCKET = -1;
constexpr int SOCKET_ERROR = -1;

}  // anonymous namespace


namespace websocket {

namespace internal {

int connect_form_hostname(const std::string& hostname, int port) {
  struct addrinfo hints;
  struct addrinfo* result;
  int ret;

  int sockfd = INVALID_SOCKET;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((ret = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints,
                         &result)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
    return 1;
  }
  for (auto info = result; result != NULL; result = result->ai_next) {
    sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    if (sockfd == INVALID_SOCKET) continue;

    if (connect(sockfd, info->ai_addr, info->ai_addrlen) == 0) {
      break;
    }
    ::close(sockfd);
    sockfd = INVALID_SOCKET;
  }
  freeaddrinfo(result);
  return sockfd;
}

int parse_url(const std::string& url, std::string* host, int* port) {
  *host = "echo.websocket.org";
  *port = 80;

  return true;
}

}  // namespace internal

}  // namespace websocket

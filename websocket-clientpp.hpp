#include <functional>
#include <string>

namespace websocket {

namespace internal {

int connect_form_hostname(const std::string& hostname, int port); 

int parse_url(const std::string& url, std::string* host, int* port);

}  // namespace internal


class WebSocket {

 public:
  WebSocket(const std::string& url);

  static WebSocket* create_connection(const std::string& url) {
    WebSocket* ws = new WebSocket(url);
    
    std::string host;
    int port;
    internal::parse_url(url, &host, &port);

    ws->sockfd_ = internal::connect_form_hostname(host, port);
    return ws;
  }

 private:
  int sockfd_;
};

inline WebSocket* create_connection(const std::string& url) {
  return WebSocket::create_connection(url);
}

}  // namespace websocket

#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <csignal>
#include <netdb.h>
#include <unordered_set>
#include <sstream>
#include <sys/poll.h>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

#include "Proxy.h"

const size_t BUFFER_SIZE = 4096;

const int TIMEOUT = 10;

const std::string error_message = "HTTP/1.1 400 Bad Request\r\n"
                                  "Connection: Closed\r\n\r\n";

const std::string method_not_allowed = "HTTP/1.1 405 Method Not Allowed\r\n"
                                       "Connection: Closed\r\n\r\n";

Proxy::Proxy(int port, int num_of_listeners) : stop_(false), num_of_listeners(num_of_listeners) {
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (server_socket < 0) {
    throw std::runtime_error("can't create socket");
  }

  sockaddr_in my_address = {};
  memset((char *)&my_address, 0, sizeof(my_address));

  my_address.sin_family = AF_INET;
  my_address.sin_port = htons(port);
  my_address.sin_addr.s_addr = inet_addr("127.0.0.1");

  int err = bind(server_socket, (struct sockaddr *)&my_address, sizeof(my_address));

  if (err < 0) {
    close(server_socket);
    throw std::runtime_error("can't bind port " + std::to_string(port));
  }

  std::cout << "my address: " << inet_ntoa(my_address.sin_addr) << ":" << ntohs(my_address.sin_port) << std::endl;

}

int Proxy::run() {

  listen(server_socket, num_of_listeners);

  while (!stop_) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server_socket, &readfds);

    struct timeval tv {TIMEOUT, 0};

    if (select(server_socket + 1, &readfds, nullptr, nullptr, &tv) == 0) {
      continue;
    }

    struct sockaddr client_address;
    int client_address_length = sizeof(client_address);

    int client_socket = accept(server_socket, &client_address, (socklen_t *)&client_address_length);

    if (client_socket < 0) {
      std::cout << "can't accept client. err: " << strerror(errno) << std::endl;
    }

    auto worker = std::thread([&](int client_socket) mutable { this->process_client(client_socket); }, client_socket);
    worker.detach();

  }

  return 0;
}

int Proxy::stop() {
  stop_ = true;
  return 0;
}



http::request<http::string_body> do_parse(std::string_view input) {
  beast::error_code ec;
  http::request_parser<http::string_body> p;

  auto buf = boost::asio::buffer(input);
  p.put(buf, ec);

  return p.release();

}

std::pair<std::vector<char>, STATUS> get_bytes_from_cash(std::shared_ptr<cash_data>& data, size_t already_read_num) {
  std::shared_lock<std::shared_mutex> lock(data->mutex);

  if (data->status == READY) {
    return {{data->data.begin() + already_read_num, data->data.end()}, READY};
  } else if (data->status == NOT_READY) {

    while (data->data.size() - already_read_num == 0 && data->status == NOT_READY) {
      data->cv.wait(lock);
    }

    return {{data->data.begin() + already_read_num, data->data.end()}, data->status};
  } else {
    return {{}, ERROR};
  }
}

bool is_ends_with(std::string_view str, std::string_view substr) {
  if (str.size() < substr.size()) {
    return false;
  }

  return str.substr(str.size() - substr.size()) == substr;
}

int connect(const std::string &host_name) {
  addrinfo hints {}, *addr_list;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  int err = getaddrinfo(host_name.c_str(), "80", &hints, &addr_list);

  if (err < 0) {
    std::cout << "getaddrinfo: " << gai_strerror(err) << std::endl;
    return -1;
  }

  auto *curr_addr = addr_list;
  int remote_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  while (curr_addr) {
    std::cout << "trying to connect to " << inet_ntoa(((sockaddr_in *)curr_addr->ai_addr)->sin_addr) << ":"
              << ntohs(((sockaddr_in *)curr_addr->ai_addr)->sin_port) << std::endl;
    err = connect(remote_socket, addr_list->ai_addr, addr_list->ai_addrlen);

    if (err == 0) {
      std::cout << "connected" << std::endl;
      break;
    }
    std::cout << "fail" << std::endl;
    curr_addr = curr_addr->ai_next;
  }
  freeaddrinfo(addr_list);

  if (err != 0) {
    return -1;
  }

  return remote_socket;
}

void get_data_from_remote(std::shared_ptr<cash_data> cash,
                          const std::string &host_name,
                          const std::string &request_for_server) {
  {
    std::unique_lock<std::shared_mutex> lock(cash->mutex);

    cash->data = "";
    cash->status = NOT_READY;
  }

  int remote_socket = connect(host_name);

  if (remote_socket == -1) {
    std::cout << "can't connect" << std::endl;
    cash->status = ERROR;
    cash->cv.notify_all();
    cash->time = time(nullptr);
    return;
  }

  struct timeval timeout {3, 0};

  if (setsockopt(remote_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                 sizeof timeout) < 0)
    exit(1);

  if (setsockopt(remote_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                 sizeof timeout) < 0)
    exit(1);

  send(remote_socket, request_for_server.c_str(), request_for_server.size(), MSG_CMSG_CLOEXEC);

  while (true) {
    char buffer[BUFFER_SIZE] = {};
    int bytes_read = recv(remote_socket, buffer, BUFFER_SIZE, MSG_CMSG_CLOEXEC);

    if (bytes_read <= 0) {
      std::unique_lock<std::shared_mutex> lock(cash->mutex);
      cash->status = ERROR;
      cash->time = time(nullptr);
      lock.unlock();
      cash->cv.notify_all();
      break;
    }

    if (is_ends_with(std::string_view(buffer, bytes_read), "\r\n\r\n")) {
      std::unique_lock<std::shared_mutex> lock(cash->mutex);
      cash->data += std::string_view(buffer, bytes_read);
      cash->status = READY;
      cash->time = time(nullptr);
      lock.unlock();
      cash->cv.notify_all();
      break;
    }

    std::unique_lock<std::shared_mutex> lock(cash->mutex);
    cash->data += std::string_view(buffer, bytes_read);
    lock.unlock();
    cash->cv.notify_all();
  }

  close(remote_socket);

}

int Proxy::process_request(const std::string &host_name, int client_socket, const std::string &request_for_server) {
  std::shared_ptr<cash_data> cash_record;
  {
    std::unique_lock<std::mutex> lock(cash_mutex_);

    auto it = cash_.find(request_for_server);

    if (it == cash_.end() || (time(nullptr) - it->second->time) > TIMEOUT && it->second->status == READY) {

      cash_[request_for_server] = std::make_shared<cash_data>();

      cash_record = cash_[request_for_server];

      std::thread get_data_from_remote_thread(get_data_from_remote, cash_record, host_name, request_for_server);

      get_data_from_remote_thread.detach();

    } else {
      cash_record = it->second;
    }
  }

  for (size_t i = 0;;) {
    auto [bytes, status] = get_bytes_from_cash(cash_record, i);
    if (status == ERROR) {
      return -1;
    }
    int err = send(client_socket, bytes.data(), bytes.size(), MSG_CMSG_CLOEXEC);
    if (err < 0) {
      return -1;
    }
    i += bytes.size();
    if (status == READY) {
      break;
    }
  }

  return 0;
}

bool is_closed(int socket) {
  struct pollfd pfd;
  pfd.fd = socket;
  pfd.events = POLLIN | POLLHUP | POLLRDNORM;
  pfd.revents = 0;
  if (poll(&pfd, 1, 100) > 0) {
    // if result > 0, this means that there is either data available on the
    // socket, or the socket has been closed
    char buffer[32];
    if (recv(socket, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) {
      return true;
    }
  }
  return false;
}

void Proxy::process_client(int client_socket) {

  try {
    char buffer[BUFFER_SIZE] = {};
    std::string line;
    std::string request_for_server;
    std::string host_name;
    std::stringstream stream;

    while (!is_closed(client_socket)) {
      if (recv(client_socket, buffer, BUFFER_SIZE, MSG_CMSG_CLOEXEC) < 0) {
        close(client_socket);
        return;
      }

      auto req = do_parse(buffer);

      if (req.method_string() != std::string_view("GET")) {
        send(client_socket, method_not_allowed.c_str(), method_not_allowed.size(), MSG_CMSG_CLOEXEC);
      }

      request_for_server += "GET ";
      request_for_server += req.target();
      request_for_server += " HTTP/1.1\r\n";
      request_for_server += "Host: ";
      request_for_server += req.at("Host");
      request_for_server += "\r\n";
      request_for_server += "Accept: ";
      request_for_server += req.at("Accept");
      request_for_server += "\r\n\r\n";
      host_name = req.at("Host");

      if (process_request(host_name, client_socket, request_for_server) < 0) {
        close(client_socket);
        return;
      }
    }
  } catch (const std::exception &e) {
    if(!is_closed(client_socket)) {
      send(client_socket, error_message.c_str(), error_message.size(), MSG_CMSG_CLOEXEC);
    }
    close(client_socket);
    return;
  }

  close(client_socket);
}

Proxy::~Proxy() {
  stop();
  close(server_socket);
}



#pragma once
#include <sys/socket.h>
#include <unordered_map>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>

struct cash_data {
  std::string data;
  bool is_ready = false;

  std::shared_mutex mutex;
  std::condition_variable_any cv;
  int time = 0;
};

class Proxy {
 public:

  explicit Proxy(int port, int num_of_listeners);

  int run();


  int stop();

  ~Proxy();


 private:

  void process_client(int client_socket);

  int process_request(const std::string &host_name, int client_socket, const std::string &request_for_server);

  int server_socket;

  std::atomic<bool> stop_;

  std::vector<std::thread> workers_;

  std::mutex cash_mutex_;

  std::unordered_map<std::string, std::shared_ptr<cash_data>> cash_;
  int num_of_listeners;
};


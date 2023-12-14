#include <iostream>
#include "Proxy.h"

void start_proxy(Proxy &proxy) {
	proxy.run();
}


int main() {

  int port;
  std::cin >> port;



  Proxy proxy(port, 5);


  std::thread t(start_proxy, std::ref(proxy));

  while (true) {
    std::string command;
    std::cin >> command;
    if (command == "stop") {
      proxy.stop();
      break;
    }
  }

  t.join();


  return 0;
}

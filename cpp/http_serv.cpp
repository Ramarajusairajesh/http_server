#include <arpa/inet.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define FILES_DIRECTORY "./templates/"
#define PORT 8080
#define REQUEST_SIZE 8192 // Max length of HTTP request

using std::cerr, std::cin, std::cout, std::endl, std::ofstream, std::string;

class HttpServer {
private:
  int server_fd, client_fd;
  struct sockaddr_in address;
  socklen_t address_length;

  void logRequest(const string &client_ip, const string &requestPath) {
    ofstream logs_file("logs.txt", std::ios::app);
    if (!logs_file) {
      cerr << "Error opening logs file" << endl;
      return;
    }

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;
    // Logs Format YYYY-MM-DD HH-mm-SS
    std::tm local_tm = *std::localtime(&time_t_now);

    logs_file << "[" << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "."
              << std::setw(3) << std::setfill('0') << ms.count() << "] "
              << "[" << client_ip << "] "
              << "Requested: " << requestPath << endl;

    logs_file.close();
  }

  string parseRequest(const string &request) {
    std::istringstream requestStream(request);
    string method, path, protocol;
    requestStream >> method >> path >> protocol;

    if (path == "/")
      path = "/index.html"; // Default page

    return path;
  }

  void serveFile(const string &filePath, int client_fd) {
    std::ifstream file(FILES_DIRECTORY + filePath.substr(1));
    if (!file) {
      file.open(FILES_DIRECTORY "404.html");
      if (!file) {
        cerr << "404.html not found" << endl;
        close(client_fd);
        return;
      }
    }

    std::ostringstream fileContent;
    fileContent << file.rdbuf();
    string response = "HTTP/2.0 200 OK\r\nContent-Type: text/html\r\n\r\n" +
                      fileContent.str();

    send(client_fd, response.c_str(), response.size(), 0);
    close(client_fd);
  }

public:
  HttpServer() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
      cerr << "Error creating socket" << endl;
      exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      cerr << "Error binding socket" << endl;
      exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
      cerr << "Error listening on socket" << endl;
      exit(EXIT_FAILURE);
    }

    cout << "HTTP Server running on port " << PORT << endl;
  }

  void start() {
    while (true) {
      address_length = sizeof(address);
      client_fd =
          accept(server_fd, (struct sockaddr *)&address, &address_length);
      if (client_fd < 0) {
        cerr << "Error accepting connection" << endl;
        continue;
      }

      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);
      getpeername(client_fd, (struct sockaddr *)&client_addr, &client_len);

      char client_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

      char buffer[REQUEST_SIZE] = {0};
      ssize_t bytesRead = recv(client_fd, buffer, REQUEST_SIZE, 0);
      if (bytesRead <= 0) {
        cerr << "Error reading request" << endl;
        close(client_fd);
        continue;
      }

      string request(buffer);
      string requestPath = parseRequest(request); // Extracts "/home", "/", etc.

      logRequest(client_ip, requestPath);

      serveFile(requestPath, client_fd);
    }
  }

  ~HttpServer() {
    close(server_fd);
    cout << "Server shutting down..." << endl;
  }
};

int main() {
  HttpServer server;
  server.start();
  return EXIT_SUCCESS;
}

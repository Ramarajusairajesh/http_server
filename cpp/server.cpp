#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

#define response_heads "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
#define not_found_heads                                                        \
  "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nFile not "         \
  "found\r\n"
#define Buffer_Size 65536
#define file_directory "./files"

// logging class that write to the file in format date: method: file: client_ip
class Logging {
private:
  mutex m;

public:
  void server_logging(const string &method, const string &file,
                      const string &client_ip) {
    lock_guard<mutex> lock(m);

    time_t now = time(0);
    char *dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0';

    ofstream logsfile("logs.txt", ios::app);
    if (logsfile.is_open()) {
      logsfile << dt << ":" << method << ":" << file << ":" << client_ip
               << endl;
      logsfile.close();
    }
  }

  /*
   *
   * Feature to do add the logs to write to a file shared via smb file share
   */
};

class http_server {
private:
  Logging log;

public:
  bool check_file(const string &filename) {
    try {
      string check_filename =
          (filename.empty() || filename == "/") ? "index.html" : filename;

      fs::path full_path =
          fs::canonical(fs::path(file_directory) / check_filename);
      fs::path base_path = fs::canonical(file_directory);

      if (full_path.string().find(base_path.string()) == 0 &&
          fs::exists(full_path)) {
        return true;
      }
    } catch (const fs::filesystem_error &e) {
      cerr << "File check error: " << e.what() << endl;
    }
    return false;
  }
  // Explanation: it receive the request via socket and check for the file name
  // if it existed in the direcrtory it transfer the file to client other wise
  // send a 404 page that we define as a macro :)
  int response(int client_socket) {
    char request[512];
    char client_ip[INET_ADDRSTRLEN];
    socklen_t addr_len;
    struct sockaddr_in client_addr;
    addr_len = sizeof(client_addr);

    getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len);
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    int request_size = read(client_socket, request, sizeof(request) - 1);

    if (request_size > 0) {
      request[request_size] = '\0';

      char method[10] = {0};
      char file_request[256] = {0};
      sscanf(request, "%s /%s", method, file_request);

      string sanitized_file = file_request;
      size_t space_pos = sanitized_file.find(' ');
      if (space_pos != string::npos) {
        sanitized_file = sanitized_file.substr(0, space_pos);
      }

      if (sanitized_file.empty() || sanitized_file == "/") {
        sanitized_file = "index.html";
      }

      string file_path = string(file_directory) + "/" + sanitized_file;

      if (check_file(sanitized_file)) {
        ifstream file(file_path, ios::in | ios::binary);
        string response = response_heads;
        send(client_socket, response.c_str(), response.size(), 0);

        char file_buffer[Buffer_Size];
        while (file.read(file_buffer, sizeof(file_buffer)) ||
               file.gcount() > 0) {
          send(client_socket, file_buffer, file.gcount(), 0);
        }

        log.server_logging(method, sanitized_file, client_ip);
      } else {
        send(client_socket, not_found_heads, strlen(not_found_heads), 0);

        log.server_logging(method, sanitized_file, client_ip);
      }
    }

    close(client_socket);
    return 0;
  }
};
// converting text input to a port number
int conversion(char *flags) {
  try {
    return stoi(flags);
  } catch (const invalid_argument &e) {
    return 0;
  }
}

int main(int argc, char **argv) {
  int port = 0;
  char log_file[64] = "";
  struct sockaddr_in address;
  int server_socket;
  int address_len = sizeof(address);
  vector<thread> threads;
  mutex thread_mutex;

  // madness started
  // You can either pass the port number or logs files name as your wish in any
  // order you want! Other wise it run on port 8080 and the logs are written to
  // the file logs.txt
  if (argc == 2) {
    int flag = conversion(argv[1]);
    if (flag > 0) {
      port = flag;
    } else {
      strncpy(log_file, argv[1], 63);
    }
  } else if (argc == 3) {
    int flag1 = conversion(argv[1]);
    int flag2 = conversion(argv[2]);
    if (flag1 > 0) {
      port = flag1;
      strncpy(log_file, argv[2], 63);
    } else {
      port = flag2;
      strncpy(log_file, argv[1], 63);
    }
  }

  if (port == 0) {
    port = 8080;
  }
  if (strlen(log_file) == 0) {
    strncpy(log_file, "logs.txt", 63);
  }

  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }
  // end of my madness
  int opt = 1;
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)) < 0) {
    perror("Error while configuring socket");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Socket binding failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  if (listen(server_socket, 20) < 0) {
    perror("Listening on socket failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  http_server server;
  cout << "Server running on port: " << port << endl;
  cout << "Logs are saved inside the file " << log_file << endl;

  while (true) {
    int client_socket = accept(server_socket, (struct sockaddr *)&address,
                               (socklen_t *)&address_len);
    if (client_socket < 0) {
      perror("Failed to accept connections");
      continue;
    }

    { // created thread with mutex since these threads write there logs to a
      // single log file
      lock_guard<mutex> lock(thread_mutex);
      if (threads.size() >= 10) {
        threads.front().join();
        threads.erase(threads.begin());
      }
      threads.emplace_back(
          thread(&http_server::response, &server, client_socket));
    }
  }

  for (auto &th : threads) {
    if (th.joinable()) {
      th.join();
    }
  }

  close(server_socket);
  return 0;
}

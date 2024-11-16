#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define port 8080
#define Buffer_size 65536
#define root_directory "./files"

struct {
  char *ext, *filetype
} extension[] = {{"gif", "image/gif"},
                 {"jpg", "image/jpg"},
                 {"jpeg", "image/jpeg"},
                 {"png", "image/png"},
                 {"ico", "image/ico"},
                 {"zip", "image/zip"},
                 {"gz", "image/gz"},
                 {"tar", "image/tar"},
                 {"htm", "text/html"},
                 {"html", "text/html"},
                 {0, 0}};

int webserver(int socket_d, char *file_name) {}

void logs(void) {}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("The server running on port 8080 and the log file is log.txt");
  } else if (argc < 3) {
    printf("The server using log.txt as default log file ");
  } else {
  }

  return 0;
}

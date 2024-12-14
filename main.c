#include <stdio.h> // fclose, FILE, fopen, fread, perror, printf, sprintf, sscanf
#include <stdlib.h> // exit, free, malloc
#include <string.h> // bzero, strlen, strncmp, strstr
#include <unistd.h> // close, read, write
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // accept, bind, listen, setsockopt, socket, socklen_t

#include "parsing.h"

int main() {
  int server_sock, client_sock;
  socklen_t cli_len;
  struct sockaddr_in serv_addr, cli_addr;
  char buffer[BUFFER_SIZE];

  // Inisialisasi server socket
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0) {
    perror("ERROR opening socket");
    exit(EXIT_FAILURE);
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);

  if (bind(server_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
    exit(EXIT_FAILURE);
  }

  if (listen(server_sock, 5) < 0){
    perror("ERROR on listen");
    exit(EXIT_FAILURE);
  }
  cli_len = sizeof(cli_addr);

  printf("Server is listening on port %d\n", PORT);
  while (1) {
    client_sock = accept(server_sock, (struct sockaddr *)&cli_addr, &cli_len);
    if (client_sock < 0) {
      perror("ERROR on accept");
      continue;
    }

    if (fork() == 0) {
      close(server_sock);
      bzero(buffer, BUFFER_SIZE);
      read(client_sock, buffer, sizeof(buffer) - 1);
      handle_http_request(client_sock, buffer);
      close(client_sock);
      exit(0);
    }
    close(client_sock);
  }

  close(server_sock);
  return 0;
}

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "server.h"

int main(){
  int server_fd;
  struct sockaddr_in server_addr;

  // Membuat server socket
  if ((server_fd = socket(AF_INET, SOC_STREAM, 0)) < 0) {
    perror("Gagal membuat socket");
    exit(EXIT_FAILURE);
  }

  // Konfigurasi socket
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind socet ke port
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind gagal");
    exit(EXIT_FAILURE);
  }

  // Listen connections
  if (listen(server_fd, 10) < 0) {
    perror("Listen gagal");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on PORT: %d\n", PORT);

  while (1) {
    // Info client
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int *client_fd = malloc(sizeof(int));

    // Accept koneksi dari client
    if ((*client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
      perror("Accept gagal");
      exit(EXIT_FAILURE);
    }

    // Membuat fork() untuk melakukan handle client
    pid_t pid = fork();

    if (pid < 0) {
      perror("Fork gagal");
      close(*client_fd);
      free(client_fd);
      continue;
    } else if (pid == 0) {
      // Child process untuk meng-handle client
      close(server_fd);
      handle_client(*client_fd);
      free(client_fd);
      exit(0);
    } else {
      // Parent process
      close(*client_fd);
      free(client_fd);
      wait(NULL);
    }
  }

  close(server_fd);
  return 0;
}

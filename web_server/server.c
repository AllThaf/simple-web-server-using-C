#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

    // Parse HTTP request to get the requested file
    char method[8], path[1024];
    sscanf(buffer, "%s %s", method, path);

    // Default to index.html if no file is specified
    if (strcmp(path, "/") == 0) {
        strcpy(path, "/index.html");
    }

    // Build full file path
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "www%s", path);

    // Check if file exists
    int file_fd = open(full_path, O_RDONLY);
    if (file_fd == -1) {
        // Send 404 Not Found response
        const char *response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 62\r\n"
            "\r\n"
            "<html><body><h1>404 Not Found</h1></body></html>";
        send(client_socket, response, strlen(response), 0);
    } else {
        // Get file size
        struct stat file_stat;
        fstat(file_fd, &file_stat);

        // Send HTTP response headers
        char headers[BUFFER_SIZE];
        snprintf(headers, sizeof(headers),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: %ld\r\n"
                 "\r\n",
                 file_stat.st_size);
        send(client_socket, headers, strlen(headers), 0);

        // Send file content
        char file_buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(file_fd, file_buffer, BUFFER_SIZE)) > 0) {
            send(client_socket, file_buffer, bytes_read, 0);
        }
        close(file_fd);
    }

    close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Client accept failed");
            continue;
        }

        if (fork() == 0) {
            // Child process handles the client
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        } else {
            // Parent process closes the client socket
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}


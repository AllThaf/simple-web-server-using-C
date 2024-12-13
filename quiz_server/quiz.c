#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int client_count = 0;
int client_sockets[MAX_CLIENTS];
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(const char *message) {
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
        send(client_sockets[i], message, strlen(message), 0);
    }
    pthread_mutex_unlock(&client_mutex);
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char name[50];

    // Receive client's name
    recv(client_socket, name, sizeof(name), 0);
    pthread_mutex_lock(&client_mutex);
    client_sockets[client_count++] = client_socket;
    pthread_mutex_unlock(&client_mutex);

    char join_message[BUFFER_SIZE];
    snprintf(join_message, sizeof(join_message), "%s has joined the quiz!\n", name);
    printf("%s", join_message);
    broadcast_message(join_message);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }
        char message[BUFFER_SIZE];
        snprintf(message, sizeof(message), "%s: %s", name, buffer);
        printf("%s", message);
        broadcast_message(message);
    }

    close(client_socket);
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == client_socket) {
            for (int j = i; j < client_count - 1; j++) {
                client_sockets[j] = client_sockets[j + 1];
            }
            break;
        }
    }
    client_count--;
    pthread_mutex_unlock(&client_mutex);

    snprintf(join_message, sizeof(join_message), "%s has left the quiz.\n", name);
    printf("%s", join_message);
    broadcast_message(join_message);

    return NULL;
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        socklen_t client_len = sizeof(client_addr);
        int *client_socket = malloc(sizeof(int));
        *client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

        if (*client_socket < 0) {
            perror("Accept failed");
            free(client_socket);
            continue;
        }

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, client_socket);
        pthread_detach(thread_id);
    }

    close(server_socket);
    return 0;
}


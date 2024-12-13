#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8000
#define BUFFER_SIZE 1024

typedef struct {
    char question[256];
    char answer[50];
} Quiz;

Quiz questions[] = {
    {"What is the capital of France?", "Paris"},
    {"What is 5 + 7?", "12"},
    {"What is the programming language of this server?", "C"}
};

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    int score = 0;

    for (int i = 0; i < sizeof(questions)/sizeof(questions[0]); i++) {
        send(client_fd, questions[i].question, strlen(questions[i].question), 0);
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline

        if (strcasecmp(buffer, questions[i].answer) == 0) {
            score++;
            send(client_fd, "Correct!\n", 9, 0);
        } else {
            send(client_fd, "Wrong!\n", 7, 0);
        }
    }

    snprintf(buffer, BUFFER_SIZE, "Your final score: %d\n", score);
    send(client_fd, buffer, strlen(buffer), 0);
    close(client_fd);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("Accept failed");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd);
            handle_client(client_fd);
            exit(0);
        } else {
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}

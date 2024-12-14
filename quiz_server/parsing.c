#include <stdio.h> // fclose, FILE, fopen, fread, perror, printf, sprintf, sscanf
#include <stdlib.h> // exit, free, malloc
#include <string.h> // bzero, strlen, strncmp, strstr
#include <unistd.h> // close, read, write
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // accept, bind, listen, setsockopt, socket, socklen_t

#include <stdbool.h> // Untuk boolean

#define BUFFER_SIZE 8000
#define PORT 8000

void error(const char *msg) {
    perror(msg);
    exit(1);
}

bool is_correct(const char *answer, const char *correct_answer) {
    return strcmp(answer, correct_answer) == 0;
}

void handle_http_request(int client_socket, char *request) {
    char response[BUFFER_SIZE];
    if (strncmp(request, "POST /", 6) == 0) {
        // Parsing data dari body
        char *body = strstr(request, "\r\n\r\n");
        if (body) {
            body += 4; // Pindah ke awal body
            printf("Data received: %s\n", body);

            // Validasi dan hitung skor
            char playerName[100], q1[100], q2[100], q3[100], q4[100], q5[100];
            int score = 0;

            sscanf(body, "playerName=%[^&]&q1=%[^&]&q2=%[^&]&q3=%[^&]&q4=%[^&]&q5=%s",
                   playerName, q1, q2, q3, q4, q5);

            if (is_correct(q1, "4")) score += 10; // Jawaban benar: 4
            if (is_correct(q2, "paris")) score += 10; // Jawaban benar: paris
            if (is_correct(q3, "4")) score += 10; // Jawaban benar: 4
            if (is_correct(q4, "shakespeare")) score += 10; // Jawaban benar: shakespeare
            if (is_correct(q5, "30")) score += 10; // Jawaban benar: 30

            printf("Player: %s, Score: %d\n", playerName, score);

            // Kirim respons ke klien
            sprintf(response,
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: keep-alive\r\n\r\n"
                    "Player: %s, Your Score: %d\n",
                    playerName, score);
            send(client_socket, response, strlen(response), 0);
        } else {
            sprintf(response,
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n\r\n"
                    "Bad request.\n");
            send(client_socket, response, strlen(response), 0);
        }
    } else if (strncmp(request, "GET /", 5) == 0) {
        // Kirim file HTML sebagai respons
        FILE *html_file = fopen("quiz.html", "r");
        if (html_file) {
            char html_content[BUFFER_SIZE];
            size_t bytes_read = fread(html_content, 1, sizeof(html_content) - 1, html_file);
            html_content[bytes_read] = '\0'; // Null-terminate string
            fclose(html_file);

            sprintf(response,
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n\r\n"
                    "%s",
                    html_content);
            send(client_socket, response, strlen(response), 0);
        } else {
            sprintf(response,
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n\r\n"
                    "404 Not Found\n");
            send(client_socket, response, strlen(response), 0);
        }
    } else {
        // Handle request tidak valid
        sprintf(response,
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n\r\n"
                "Unsupported request method.\n");
        send(client_socket, response, strlen(response), 0);
    }
}

int main() {
    int server_sock, client_sock;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[BUFFER_SIZE];

    // Inisialisasi server socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(server_sock, 5);
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


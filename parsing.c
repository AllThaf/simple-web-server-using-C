#include <stdio.h> // fclose, FILE, fopen, fread, perror, printf, sprintf, sscanf
#include <stdlib.h> // exit, free, malloc
#include <string.h> // bzero, strlen, strncmp, strstr
#include <unistd.h> // close, read, write
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // accept, bind, listen, setsockopt, socket, socklen_t

#include "parsing.h"

void handle_request_post(int client_socket, char *response, char *request){
  // Parsing data dari body
  char *body = strstr(request, "\r\n\r\n");
  if (body) {
    body += 4; // Pindah ke awal body
    printf("Data received: %s\n", body);

    // Validasi dan hitung skor
    char playerName[100], q1[100], q2[100], q3[100], q4[100], q5[100], q6[100], q7[100], q8[100], q9[100], q10[100];
    int score = 0;

    sscanf(body, "playerName=%[^&]&q1=%[^&]&q2=%[^&]&q3=%[^&]&q4=%[^&]&q5=%[^&]&q6=%[^&]&q7=%[^&]&q8=%[^&]&q9=%[^&]&q10=%s",
          playerName, q1, q2, q3, q4, q5, q6, q7, q8, q9, q10);

    if (strcmp(q1, "4") == 0) score += 10; // Jawaban benar: 4
    if (strcmp(q2, "Paris") == 0) score += 10; // Jawaban benar: Paris
    if (strcmp(q3, "4") == 0) score += 10; // Jawaban benar: 4
    if (strcmp(q4, "Australia") == 0) score += 10; // Jawaban benar: Australia
    if (strcmp(q5, "30") == 0) score += 10; // Jawaban benar: 30
    if (strcmp(q6, "Soekarno") == 0) score += 10; // Jawaban benar: Sukarno
    if (strcmp(q7, "Tokyo") == 0) score += 10; // Jawaban benar: Tokyo
    if (strcmp(q8, "Everest") == 0) score += 10; // Jawaban benar: Mount Everest
    if (strcmp(q9, "Merkurius") == 0) score += 10; // Jawaban benar: Mercury
    if (strcmp(q10, "Jawa") == 0) score += 10; // Jawaban benar: Jawa
    printf("Player: %s, Score: %d\n", playerName, score); 

    // Kirim respons ke klien
    sprintf(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: keep-alive\r\n\r\n"
            "Player: %s, Your Score: %d\n",
            playerName, score);
    send(client_socket, response, strlen(response), 0);

    FILE *file = fopen("scores.txt", "a");
    if (file) {
        fprintf(file, "%s: %d\n", playerName, score);
        fclose(file);
    }

  } else {
    sprintf(response,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n\r\n"
            "Bad request.\n");
    send(client_socket, response, strlen(response), 0);
  }
}

void handle_request_getscore(int client_socket, char *response) {
  // Baca file highscore
  FILE *file = fopen("scores.txt", "r");
  if (!file) {
      sprintf(response,
              "HTTP/1.1 500 Internal Server Error\r\n"
              "Content-Type: text/plain\r\n"
              "Connection: close\r\n\r\n"
              "Failed to open scores.txt\n");
      send(client_socket, response, strlen(response), 0);
      return;
  }

  // Array untuk menyimpan skor
  highScoreTemp scores[100];
  int count = 0;

  char line[256];
  while (fgets(line, sizeof(line), file)) {
      // Parsing format "nama: score"
      char name[100];
      int score;
      if (sscanf(line, "%[^:]: %d", name, &score) == 2) {
          strcpy(scores[count].name, name);
          scores[count].score = score;
          count++;
      }
  }
  fclose(file);

  // Sort array secara descending
  for (int i = 0; i < count - 1; i++) {
      for (int j = i + 1; j < count; j++) {
          if (scores[i].score < scores[j].score) {
              // Tukar elemen
              highScoreTemp temp = scores[i];
              scores[i] = scores[j];
              scores[j] = temp;
          }
      }
  }

  // Gabungkan hasil ke dalam satu string
  char file_content[BUFFER_SIZE] = {0};
  for (int i = 0; i < count; i++) {
      char entry[128];
      sprintf(entry, "%s: %d\n", scores[i].name, scores[i].score);
      strcat(file_content, entry);
  }

  // Kirim respons ke klien
  sprintf(response,
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/plain\r\n"
          "Connection: close\r\n\r\n"
          "%s",
          file_content);
  send(client_socket, response, strlen(response), 0);
}

void handle_request_get(int client_socket, char *response){
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
}

void handle_http_request(int client_socket, char *request) {
  char response[BUFFER_SIZE];
  if (strncmp(request, "POST /", 6) == 0) {
    handle_request_post(client_socket, response, request);
  } else if (strncmp(request, "GET /scores", 11) == 0) {
    handle_request_getscore(client_socket, response);
  } else if (strncmp(request, "GET /", 5) == 0) {
    handle_request_get(client_socket, response);
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


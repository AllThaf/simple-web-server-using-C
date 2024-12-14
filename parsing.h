#ifndef PARSING_H
#define PARSING_H

#define BUFFER_SIZE 8192
#define PORT 8000

extern void handle_http_request(int client_socket, char *request);

#endif // !PARSING_H

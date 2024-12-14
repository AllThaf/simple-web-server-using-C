#ifndef PARSING_H
#define PARSING_H

#define BUFFER_SIZE 8192
#define PORT 8000

/*!
* Untuk menghanlde request client dan meresponnya sesuai reuqest
*
* @param client_socket Mewakili soket komunikasi antara server dan client
* @param *request Request yang diberikan oleh clien
*/
extern void handle_http_request(int client_socket, char *request);

#endif // !PARSING_H

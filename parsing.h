#ifndef PARSING_H
#define PARSING_H

#define BUFFER_SIZE 8192
#define PORT 8000

typedef struct {
    char name[100];
    int score;
} highScoreTemp;

/*!
* Untuk menghandle request client dan meresponnya sesuai request
*
* @param client_socket Mewakili soket komunikasi antara server dan client
* @param *request Buffer yang berisi HTTP request dari client
*/
extern void handle_http_request(int client_socket, char *request);

/*!
* Untuk menghandle request dengan method POST
*
* @param client_socket Mewakili soket komunikasi antara server dan client
* @param *response Buffer untuk mentimpan reson HTTP yang akan dikirimkan
* @param *request Buffer yang berisi HTTP request dari client
*/
extern void handle_request_post(int client_socket, char *response, char *request);

/*!
* Untuk menghandle request dengan method GET /score
*
* @param client_socket Mewakili soket komunikasi antara server dan client
* @param *response Buffer untuk mentimpan reson HTTP yang akan dikirimkan
*/
extern void handle_request_getscore(int client_socket, char *response);

/*!
* Untuk menghandle request dengan method GET /
*
* @param client_socket Mewakili soket komunikasi antara server dan client
* @param *response Buffer untuk mentimpan reson HTTP yang akan dikirimkan
*/
extern void handle_request_get(int client_socket, char *response);

#endif // !PARSING_H

#ifndef SERVER_H
#define SERVER_H

#define PORT 8080

/*!
 * Untuk mengembalikan respon kepada pengguna dengan membaca file HTML yang ada
 * @param file_name Nama dari file yang akan dibaca
 * @param file_ext Extension dari file
 * @param response Buffer tempat respon HTTP yang dibangun akan disalin
 * @param response_len Panjang total respon HTTP
 */
extern void build_http_response(const char *file_name, const char *file_ext, char *response, size_t *response_len);

/*!
 * Untuk meng-handle client
 * @param arg Pointer ke data yang relevan dengan client
 * @return NULL Menandakan bahwa thread telah selesai
 */
extern void *handle_client(void *arg);

#endif // !SERVER_H

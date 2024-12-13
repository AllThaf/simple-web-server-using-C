#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <stdint.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Fungsi untuk encoding Base64
void base64_encode(const uint8_t *input, size_t len, char *output) {
    const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i, j;
    for (i = 0, j = 0; i < len;) {
        uint32_t octet_a = i < len ? input[i++] : 0;
        uint32_t octet_b = i < len ? input[i++] : 0;
        uint32_t octet_c = i < len ? input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j++] = base64_chars[(triple >> 18) & 0x3F];
        output[j++] = base64_chars[(triple >> 12) & 0x3F];
        output[j++] = (i > len + 1) ? '=' : base64_chars[(triple >> 6) & 0x3F];
        output[j++] = (i > len) ? '=' : base64_chars[triple & 0x3F];
    }
    output[j] = '\0';
}

// Dekode frame WebSocket
int decode_websocket_frame(const uint8_t *frame, uint8_t *decoded, size_t length) {
    if (length < 2) return -1; // Frame terlalu pendek
    size_t payload_len = frame[1] & 0x7F;
    size_t offset = 2;

    if (payload_len == 126) {
        payload_len = (frame[2] << 8) | frame[3];
        offset += 2;
    } else if (payload_len == 127) {
        payload_len = 0;
        for (int i = 0; i < 8; i++) {
            payload_len |= ((uint64_t)frame[2 + i]) << (8 * (7 - i));
        }
        offset += 8;
    }

    // Masking key (4 bytes)
    uint8_t mask[4];
    memcpy(mask, frame + offset, 4);
    offset += 4;

    // Decode payload
    for (size_t i = 0; i < payload_len; i++) {
        decoded[i] = frame[offset + i] ^ mask[i % 4];
    }

    return payload_len;
}

// Encode frame WebSocket
int encode_websocket_frame(const uint8_t *message, size_t message_len, uint8_t *frame) {
    frame[0] = 0x81; // FIN = 1, Opcode = 1 (text frame)
    size_t offset = 1;

    if (message_len <= 125) {
        frame[offset++] = message_len;
    } else if (message_len <= 65535) {
        frame[offset++] = 126;
        frame[offset++] = (message_len >> 8) & 0xFF;
        frame[offset++] = message_len & 0xFF;
    } else {
        frame[offset++] = 127;
        for (int i = 7; i >= 0; i--) {
            frame[offset++] = (message_len >> (8 * i)) & 0xFF;
        }
    }

    memcpy(frame + offset, message, message_len);
    return offset + message_len;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    recv(client_socket, buffer, BUFFER_SIZE, 0);

    // Cari Sec-WebSocket-Key
    char *key_start = strstr(buffer, "Sec-WebSocket-Key: ");
    if (!key_start) {
        close(client_socket);
        return;
    }
    key_start += strlen("Sec-WebSocket-Key: ");
    char *key_end = strstr(key_start, "\r\n");
    char key[128] = {0};
    strncpy(key, key_start, key_end - key_start);

    // Tambahkan magic string
    strcat(key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

    // Hash SHA1
    uint8_t hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)key, strlen(key), hash);

    // Encode Base64
    char accept_key[128];
    base64_encode(hash, SHA_DIGEST_LENGTH, accept_key);

    // Kirim respons handshake WebSocket
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 101 Switching Protocols\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Accept: %s\r\n\r\n",
             accept_key);
    send(client_socket, response, strlen(response), 0);

    printf("Client connected via WebSocket!\n");

    while (1) {
        uint8_t buffer[BUFFER_SIZE] = {0};
        uint8_t decoded[BUFFER_SIZE] = {0};

        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;

        int decoded_len = decode_websocket_frame(buffer, decoded, bytes_received);
        if (decoded_len > 0) {
            printf("Received message: %s\n", decoded);

            // Kirim pesan kembali ke client
            uint8_t encoded[BUFFER_SIZE] = {0};
            int encoded_len = encode_websocket_frame(decoded, decoded_len, encoded);
            send(client_socket, encoded, encoded_len, 0);
        }
    }

    close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
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
            // Proses anak
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        } else {
            // Proses induk
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}


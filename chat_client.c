#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int client_socket;
char username[50];

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            printf("Desconectado del servidor.\n");
            break;
        }
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error al crear socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP local para pruebas

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar al servidor");
        return 1;
    }

    printf("Ingrese su nombre de usuario: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    send(client_socket, username, strlen(username), 0);

    printf("Conectado al chat. Escriba 'exit' o 'quit' para salir.\n");

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    char message[BUFFER_SIZE];
    while (1) {
        fgets(message, sizeof(message), stdin);
        if (strcmp(message, "exit\n") == 0 || strcmp(message, "quit\n") == 0) {
            send(client_socket, message, strlen(message), 0);
            break;
        }
        send(client_socket, message, strlen(message), 0);
    }

    close(client_socket);
    return 0;
}

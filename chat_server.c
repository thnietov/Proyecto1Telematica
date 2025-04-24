#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char username[50];
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
FILE *log_file;

void log_event(const char *event) {
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp)-1] = '\0'; // Quitar \n

    pthread_mutex_lock(&clients_mutex);
    fprintf(log_file, "[%s] %s\n", timestamp, event);
    fflush(log_file);
    pthread_mutex_unlock(&clients_mutex);
}

void broadcast_message(const char *message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].socket != sender_socket) {
            if (send(clients[i].socket, message, strlen(message), 0) < 0) {
                perror("Error al enviar mensaje");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].socket == socket) {
            for (int j = i; j < client_count - 1; ++j) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);

    char buffer[BUFFER_SIZE];
    char username[50];

    // Recibir nombre de usuario
    int bytes_received = recv(client_socket, username, sizeof(username), 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return NULL;
    }
    username[bytes_received] = '\0';

    // Agregar cliente a la lista
    pthread_mutex_lock(&clients_mutex);
    clients[client_count].socket = client_socket;
    strncpy(clients[client_count].username, username, sizeof(username));
    client_count++;
    pthread_mutex_unlock(&clients_mutex);

    // Notificar entrada
    char join_msg[BUFFER_SIZE];
    snprintf(join_msg, sizeof(join_msg), ">> %s se ha unido al chat.\n", username);
    log_event(join_msg);
    broadcast_message(join_msg, client_socket);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (received <= 0) {
            break;
        }

        buffer[received] = '\0';
        if (strcmp(buffer, "exit\n") == 0 || strcmp(buffer, "quit\n") == 0) {
            break;
        }

        char full_msg[BUFFER_SIZE + 50];
        snprintf(full_msg, sizeof(full_msg), "[%s]: %s", username, buffer);
        log_event(full_msg);
        broadcast_message(full_msg, client_socket);
    }

    // Cliente se desconectó
    snprintf(join_msg, sizeof(join_msg), "<< %s salió del chat.\n", username);
    log_event(join_msg);
    broadcast_message(join_msg, client_socket);
    remove_client(client_socket);
    close(client_socket);
    return NULL;
}

void sigint_handler(int sig) {
    printf("\nCerrando servidor...\n");
    if (log_file) fclose(log_file);
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);
    log_file = fopen("chat.log", "a");
    if (!log_file) {
        perror("No se pudo abrir el archivo de log");
        return 1;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error al crear socket");
        return 1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(12345),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al hacer bind");
        return 1;
    }

    if (listen(server_socket, 10) < 0) {
        perror("Error al escuchar");
        return 1;
    }

    printf("Servidor escuchando en el puerto 12345...\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int *client_socket = malloc(sizeof(int));
        *client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);

        if (*client_socket < 0) {
            perror("Error al aceptar conexión");
            free(client_socket);
            continue;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, client_socket) != 0) {
            perror("Error al crear hilo");
            free(client_socket);
        }

        pthread_detach(tid); // Limpieza automática de hilos
    }

    close(server_socket);
    fclose(log_file);
    return 0;
}

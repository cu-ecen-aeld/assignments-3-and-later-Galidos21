#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <fcntl.h>

#define PORT 9000
#define FILE_PATH "/var/tmp/aesdsocketdata"
#define BUFFER_SIZE 1024

int server_fd = -1, client_fd = -1;

void cleanup_and_exit(int signo) {
    syslog(LOG_INFO, "Caught signal, exiting");

    if (client_fd != -1) close(client_fd);
    if (server_fd != -1) close(server_fd);
    remove(FILE_PATH);

    closelog();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);

    openlog("aesdsocket", LOG_PID, LOG_USER);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        syslog(LOG_ERR, "Socket creation failed");
        return -1;
    }

    // Allow reuse of the address
    /*int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        syslog(LOG_ERR, "setsockopt failed");
        close(server_fd);
        return -1;
    }*/
    
    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));  // ADD THIS
    

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        syslog(LOG_ERR, "Bind failed");
        close(server_fd);
        return -1;
    }
    
    int daemon_mode = 0;
    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        daemon_mode = 1;
    }
    
    if (daemon_mode) {
        pid_t pid = fork();
        if (pid < 0) {
            syslog(LOG_ERR, "Fork failed");
            return -1;
        }
        if (pid > 0) {
            exit(0); // Parent exits
        }
    
        if (setsid() < 0) {
            syslog(LOG_ERR, "setsid failed");
            return -1;
        }
    
        // Redirect stdin, stdout, stderr to /dev/null
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_WRONLY);
        open("/dev/null", O_RDWR);
    
        syslog(LOG_INFO, "Running in daemon mode");
    }

    if (listen(server_fd, 5) == -1) {
        syslog(LOG_ERR, "Listen failed");
        close(server_fd);
        return -1;
    }

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd == -1) {
            syslog(LOG_ERR, "Accept failed");
            continue;
        }

        syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(client_addr.sin_addr));

        FILE *file = fopen(FILE_PATH, "a+");
        if (!file) {
            syslog(LOG_ERR, "File open failed");
            close(client_fd);
            continue;
        }

        ssize_t bytes;
        size_t total = 0;
        char *recv_buffer = NULL;

        while ((bytes = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) {
            char *temp = realloc(recv_buffer, total + bytes + 1);
            if (!temp) {
                syslog(LOG_ERR, "Memory allocation failed");
                free(recv_buffer);
                break;
            }
            recv_buffer = temp;
            memcpy(recv_buffer + total, buffer, bytes);
            total += bytes;
            recv_buffer[total] = '\0';

            if (memchr(buffer, '\n', bytes)) break;
        }

        if (recv_buffer) {
            fwrite(recv_buffer, 1, total, file);
            fflush(file);
        }

        rewind(file);
        while ((bytes = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            send(client_fd, buffer, bytes, 0);
        }

        fclose(file);
        free(recv_buffer);

        syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(client_addr.sin_addr));
        close(client_fd);
        client_fd = -1;
    }

    return 0;
}

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

int main() {
    struct sockaddr_in addr;
    struct sockaddr_in client;
    int yes = 1;

    int sock0 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock0 < 0) {
        perror("socket");
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes,
               sizeof(yes));

    if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("bind");
        return 1;
    }

    if (listen(sock0, 5) != 0) {
        perror("listen");
        return 1;
    }

    // 応答用HTTPメッセージ作成
    char buf[2048];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf),
             "HTTP/1.0 200 OK\r\n"
             "Content-Length: 20\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "HELLO\r\n");

    while (true) {
        int len = sizeof(client);
        int sock = accept(sock0, (struct sockaddr *)&client, (socklen_t *)&len);
        if (sock < 0) {
            perror("accept");
            break;
        }
        char inbuf[2048];
        memset(inbuf, 0, sizeof(inbuf));
        recv(sock, inbuf, sizeof(inbuf), 0);
        printf("%s", inbuf);
        send(sock, buf, (int)strlen(buf), 0);

        close(sock);
    }

    close(sock0);

    return 0;
}

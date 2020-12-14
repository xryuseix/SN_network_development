#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

/* エラー処理 */
inline void errout(int status, std::string func) {
    if (status == -1) {
        perror(func.c_str());
        printf("%d\n", errno);
        exit(EXIT_FAILURE);
    }
}

/* データを送信 */
template <class T>
bool send(T sock, std::string s) {
    /* 5文字送信 */
    int write_res = write(sock, s.c_str(), s.size());
    errout(write_res, "write");
    if (s == "quit") {
        return 0;
    } else {
        return 1;
    }
}

/* データを受信 */
template <class T>
bool receive(T sock) {
    char buf[32];
    memset(buf, 0, sizeof(buf));
    int read_size = read(sock, buf, sizeof(buf));
    errout(read_size, "read");

    printf("%s\n", buf);
    if ((std::string)buf == "quit") {
        return 0;
    } else {
        return 1;
    }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server;

    int port_num = 12345;

    /* ソケットの作成 */
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    /* 接続先指定用構造体の準備 */
    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);

    /* 127.0.0.1はlocalhost */
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);

    /* サーバに接続 */
    int connect_res = connect(sock, (struct sockaddr *)&server, sizeof(server));
    errout(connect_res, "connect");

    while (true) {
        {
            std::cout << "受信中です..." << std::endl;
            bool res = receive(sock);
            if (!res) {
                // quitが送られた
                break;
            }
        }
        {
            std::cout << "送信したい文字列を入力してください" << std::endl;
            std::string s;
            std::cin >> s;
            bool res = send(sock, s);
            if (!res) {
                // quitが送られた
                break;
            }
        }
    }

    /* socketの終了 */
    close(sock);

    return 0;
}

/*
実行コマンド
[サーバー側]
gpp++ -o simpleTCPserver simpleTCPserver.cpp
./simpleTCPserver

[クライアント側]
gpp++ -o simpleTCPclient simpleTCPclient.cpp
./simpleTCPclient
*/
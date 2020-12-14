#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
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

int main() {
    struct sockaddr_in addr;
    struct sockaddr_in client;

    /* ソケットの作成 */
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);
    errout(sock0, "socket");

    /* ソケットの設定 */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    int bind_res = bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    errout(bind_res, "bind");

    /* TCPクライアントからの接続要求を待てる状態にする */
    int listen_res = listen(sock0, 5);
    errout(listen_res, "listen");

    /* TCPクライアントからの接続要求を受け付ける */
    socklen_t len = sizeof(client);
    int sock = accept(sock0, (struct sockaddr *)&client, &len);
    errout(sock, "accept");

    while (true) {
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
        {
            std::cout << "受信中です..." << std::endl;
            bool res = receive(sock);
            if (!res) {
                // quitが送られた
                break;
            }
        }
    }

    /* TCPセッションの終了 */
    close(sock);
    /* listen するsocketの終了 */
    close(sock0);

    return 0;
}

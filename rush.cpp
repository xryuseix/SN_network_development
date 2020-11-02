#include <err.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <sstream>
#include <vector>
using namespace std;

// 子プロセスを管理する構造体
struct child {
    int status;
    bool background;
};

// 実行中の子プロセス
map<pid_t, child> jobs;

/* 一行読み込み */
inline string simple_getline() {
    string s;
    getline(cin, s);
    return s;
}

/* 文字列の分割 */
bool split_line(const string &input, const char delimiter,
                vector<string> &data) {
    stringstream stream(input);
    string field;
    bool background = false;
    while (getline(stream, field, delimiter)) {
        data.push_back(field);
        background |= field == "&";
    }
    return background;
}

/* jobsの一覧を表示 */
inline void print_jobs(map<pid_t, child> v) {
    cout << "=========" << endl;
    cout << "size : " << v.size() << endl;
    for (auto s : v) {
        cout << s.first << " " << s.second.status << " " << s.second.background
             << endl;
    }
    cout << "=========" << endl;
}

void wait_forground_pid(pid_t &pid, pid_t &wpid, int &status) {
    while ((wpid = waitpid(pid, &status, 0)) > 0) {
        if (WIFEXITED(status) != 0) {
            // 子プロセスが正常終了の場合
            jobs.erase(pid);
            cout << "[FG] Jobs delete : " << pid << endl;
        }
    }
}

/* コマンド実行, 戻り値は正常/異常終了, 終了命令の有無 */
pair<bool, bool> rush_execute(vector<string> args, bool background) {
    // 引数になにも与えられなかった場合
    if (!args.size()) {  // 改行のみが与えられたとき
        return {true, false};
    }
    if (args[0] == "exit" || args[0] == "quit" || args[0] == "q") {  // 終了処理
        return {true, true};
    }
    if (args[0] == "jobs") {
        for (auto it = jobs.begin(); it != jobs.end(); ++it) {
            cout << "---------" << endl;
            cout << "pid: " << it->first << endl;
            cout << "status: " << it->second.status << endl;
            cout << "background: " << it->second.background << endl;
        }
        cout << "---------" << endl;
        return {true, false};
    } else if (args.size() >= 2 && args[0] == "fg") {
        pid_t wpid;
        pid_t pid = strtoll(args[1].c_str(), NULL, 10);
        int status = 0;
        wait_forground_pid(pid, wpid, status);
        return {true, false};
    }
    try {
        pid_t pid, wpid;
        int status = 0;

        pid = fork();

        if (pid == 0) {  // 子ノードの処理
            vector<char *> args_c;
            args_c.reserve(args.size());
            for (size_t i = 0; i < args.size(); ++i) {
                args_c.push_back(const_cast<char *>(args[i].c_str()));
            }
            args_c.push_back(NULL);

            if (execv(args[0].c_str(), args_c.data()) == -1) {
                perror("rush");
            }
            // exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Error forking
            perror("rush");
        } else {  // 親ノードの処理
            jobs[pid] = {1, background};
            cout << "Jobs add : " << pid << " size : " << jobs.size() << endl;
            if (!background) {
                wait_forground_pid(pid, wpid, status);
                // while ((wpid = waitpid(pid, &status, 0)) > 0) {
                //     if (WIFEXITED(status) != 0) {
                //         // 子プロセスが正常終了の場合
                //         jobs.erase(pid);
                //         cout << "[FG] Jobs delete : " << pid << endl;
                //     }
                // }
            }
        }
    } catch (char *str) {
        cout << "rush : " << str << endl;
        return {false, true};
    }
    return {true, false};
}

void rush_loop(void) {
    while (true) {
        {
            // バックグラウンド処理の判定
            pid_t pid, wpid;
            int status = 0;
            while ((wpid = waitpid(-1, &status, WNOHANG)) > 0) {
                if (WIFEXITED(status) != 0) {
                    jobs.erase(wpid);
                    cout << "[BG] Jobs delete : " << wpid << endl;
                }
            }
        }
        vector<string> args;
        cout << "> ";
        string line = simple_getline();
        fflush(stdin);
        bool background = split_line(line, ' ', args);

        auto [status, exit] = rush_execute(args, background);
        assert(status);
        if (exit) {
            break;
        }
    }
}

int main(int argc, char *argv[]) { rush_loop(); }

/*

/usr/local/bin/code /Users/ryuse/Desktop/
/bin/ls -l
/Users/ryuse/Desktop/College/課題/二回秋/ネットワーク開発実験/wait.out

*/
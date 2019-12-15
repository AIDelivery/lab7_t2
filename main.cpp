#include <iostream>
#include <unistd.h>     // pipe
#include <fstream>
#include <string>
#include <signal.h>
#include <wait.h>
// #include <fcntl.h>
using namespace std;

char if1_n[] = "input.txt";
char of1_n[] = "output1.txt";
char of2_n[] = "output2.txt";

char sub_prog_name[] = "./sub_prog";

int main(int argc, char* argv[]) {
    ifstream file_to_send;
    pid_t pid1, pid2;
    int filedes[2], s;
    string buff;

    file_to_send.open(if1_n, std::ifstream::in);
    pipe(filedes);


    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigaddset(&signals, SIGUSR2);
    sigaddset(&signals, SIGQUIT);
    sigprocmask(SIG_BLOCK, &signals, NULL);


    if ((pid1 = fork()) == 0)
        execl(sub_prog_name, sub_prog_name, &filedes[0], of1_n, "e", NULL);
    else if (pid1 < 0) { cout << "[Unexpected error]" << endl; exit(EXIT_FAILURE); }

    // getting SIGUSR1 = 10 from "even" process
    sigwait(&signals, &s);

    if ((pid2 = fork()) == 0)
        execl(sub_prog_name, sub_prog_name, &filedes[0], of2_n, "o", NULL);
    else if (pid2 < 0) { cout << "[Unexpected error]" << endl; exit(EXIT_FAILURE); }

    // getting SIGUSR2 = 12 from "odd" process
    // if combine both wait() -> unexpected e/o files-process match
    sigwait(&signals, &s);

    // Inition
    close(filedes[0]);
    kill(pid1, SIGUSR1);

    for (int i = 0; !file_to_send.eof(); i++) {
        getline(file_to_send, buff);
        size_t sz = buff.size();
        write(filedes[1], buff.c_str(), sz);
    }
    char t = '\0';
    write(filedes[1], &t, 1);
    write(filedes[1], &t, 1);

    // if writing was ended before SIGUSR1/SIGUSR2 signals received in subprocesses
    kill(0, SIGQUIT);
    cout << endl << endl << "[Quiting write process. Waiting for successors...]" << endl;


    waitpid(pid1, &s, 0);
    cout << endl << "[Reader #1 CLOSED]" << endl;
    waitpid(pid2, &s, 0);
    cout << "[Reader #2 CLOSED]" << endl;


    cout << endl << endl << "[Terminating...]" << endl << endl;

    close(filedes[1]);
    file_to_send.close();
    return 0;
}

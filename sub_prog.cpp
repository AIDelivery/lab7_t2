#include <iostream>
#include <sys/unistd.h>
#include <fstream>
#include <sys/signal.h>
#include <string.h>
using namespace std;

bool end_is_reached = false;

void end_register(int i) {
    end_is_reached = true;
}


int main(int argc, char* argv[]) {
    ofstream output_file;
    char buff = 'a';
    sigset_t my_signal_set, sibling_signal_set, end_of_pipe_set;
    int s = 0, sibling_signal, my_signal;
    int read_from_pipe_des = *argv[1];
    bool rdy_to_go;
    bool is_even = strcmp(argv[3], "e") == 0 ? true : false;

    output_file.open(argv[2], std::ifstream::out);

    // for "even" reading
    if (is_even) { my_signal = SIGUSR1; sibling_signal = SIGUSR2; rdy_to_go = true; }
    // for "odd" reading
    else { my_signal = SIGUSR2; sibling_signal = SIGUSR1; rdy_to_go = true; }


    sigemptyset(&my_signal_set);
    sigemptyset(&sibling_signal_set);
    sigemptyset(&end_of_pipe_set);

    sigaddset(&my_signal_set, my_signal);
    sigaddset(&sibling_signal_set, sibling_signal);
    sigaddset(&end_of_pipe_set, SIGQUIT);

    signal(SIGQUIT, end_register);

    sigprocmask(SIG_BLOCK, &sibling_signal_set, NULL);
    sigprocmask(SIG_BLOCK, &my_signal_set, NULL);
    sigprocmask(SIG_UNBLOCK, &end_of_pipe_set, NULL);

    kill(getppid(), my_signal);
    sigwait(&my_signal_set, &s);

    while (true) {
        cout << "[Process " << my_signal << " step. Writing is over? " << end_is_reached << "] ";

        if (rdy_to_go) rdy_to_go = false;
        else sigwait(&my_signal_set, &s);

        if (!read(read_from_pipe_des, &buff, 1) && end_is_reached || buff == '\0') {
            cout << endl << "[Pipe is empty and writer is dead. Terminating...]" << endl;
            break;
        }

        output_file << buff;
        cout << buff << endl;
        kill(0, sibling_signal);
    }

    kill(0, sibling_signal);
    output_file.close();
    close(read_from_pipe_des);
    exit(EXIT_SUCCESS);
}

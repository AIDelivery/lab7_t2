#include <iostream>
#include <unistd.h>
#include <fstream>
#include <signal.h>
using namespace std;

void handler(int i) {
    cout << i;
    exit(0);
}

int main(int argc, char* argv[]) {
    ofstream output_file;
    char buff;
    sigset_t signals;
    int s = 0, signal_to_react, my_signal;
    int read_from_pipe_des = *argv[1];
    bool rdy;
    
    output_file.open(argv[2], std::ifstream::out);
    
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigaddset(&signals, SIGUSR2);
    sigprocmask(SIG_UNBLOCK, &signals, NULL);
    signal(SIGUSR1, handler);
    
    cout << "INIT" << endl;
    sigwait(&signals, &s);
    cout << "END" << endl;
    
    /*
    if (sigwait(&signals, &s)) {
        cout << "[Unexpected error]" << endl;
        exit(-1);
    } else cout << "fdsaf" << endl;
    */
    
    /*
    
    cout << s << endl;
    
    if (s == SIGUSR1) {
        cout << 1 << endl;
        my_signal = SIGUSR1;
        signal_to_react = SIGUSR2;
        rdy = true;
    } else {
        cout << 2 << endl;
        my_signal = SIGUSR2;
        signal_to_react = SIGUSR1;
        rdy = false;
    }

    sigaddset(&signals, SIGQUIT);
    
    while(true) {
        if(rdy) rdy = false;
        else sigwait(&signals, &signal_to_react);
        
        int temp = read(read_from_pipe_des, &buff, 1);
        
        if(temp == -1) {
            cout << "[Unexpected error]" << endl;
            exit(-1);
        } else if(temp == 0 && s != SIGQUIT) {
            while(read(read_from_pipe_des, &buff, 1) == 0)
                usleep(10000);
        } else if(temp == 0 && s == SIGQUIT) {
            cout << "[Pipe is empty and writer is dead. Terminating...]" << endl;
            close(read_from_pipe_des);
            output_file.close();
            exit(0);
        }
        
        output_file << buff;
        cout << buff;
        kill(0, my_signal);
    }
    
    */
    
}

#include <iostream>
#include <sys/unistd.h>
#include <fstream>
#include <sys/signal.h>
#include <string.h>
using namespace std;


int main(int argc, char* argv[]) {
    ofstream output_file;
    char buff;
    sigset_t signals;
    int s, signal_to_react, my_signal;
    int read_from_pipe_des = *argv[1];
    bool rdy;
    bool is_even = strcmp(argv[3], "e") == 0 ? true : false;
    
    output_file.open(argv[2], std::ifstream::out);
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigaddset(&signals, SIGUSR2);
    sigaddset(&signals, SIGQUIT);
    sigprocmask(SIG_BLOCK, &signals, NULL);
    
//     if(is_even) sigaddset(&signals, SIGUSR2);
//     else sigaddset(&signals, SIGUSR1);
    
    
    if(is_even) kill(getppid(), SIGUSR1);
    else kill(getppid(), SIGUSR2);
    sigwait(&signals, &s);
    
    //
    cout << "Cought initial signal: " << s << " from ";
    if(is_even) cout << "SIGUSR1" << endl;
    else cout << "SIGUSR2" << endl;
    //
    
    /*
    if (sigwait(&signals, &s)) {
        cout << "[Unexpected error]" << endl;
        exit(-1);
    } else cout << "fdsaf" << endl;
    */
    
    if (is_even) {
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
    /*
    sigaddset(&signals, SIGQUIT);
    sigprocmask(SIG_BLOCK, &signals, NULL);
    
    while(true) {
        if(rdy) rdy = false;
        else sigwait(&signals, &s);
        
        int temp = read(read_from_pipe_des, &buff, 1);
        
        if(temp == -1) {
            cout << "[Unexpected error]" << endl;
            exit(-1);
        } else if(temp == 0 && s != SIGQUIT) {
            while(read(read_from_pipe_des, &buff, 1) == 0)
                usleep(100);
        } else if(temp == 0 && s == SIGQUIT) {
            cout << "[Pipe is empty and writer is dead. Terminating...]" << endl;
            break;
        }
        
        output_file << buff;
        cout << buff;
        kill(0, my_signal);
    }
    
    kill(0, my_signal);
    */
    close(read_from_pipe_des);
    output_file.close();
    exit(0);
    
}

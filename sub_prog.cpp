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
    
    if (is_even) { my_signal = SIGUSR1; sibling_signal = SIGUSR2; rdy_to_go = true; } // for even
    else { my_signal = SIGUSR2; sibling_signal = SIGUSR1; rdy_to_go = true; } // for odd
    
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
    
    cout << "1[" << my_signal << "==" << s << "] " << end_is_reached << endl;
    
    while(true) {
        if(rdy_to_go) rdy_to_go = false;
        else {cout << my_signal << " is waiting" << endl; sigwait(&my_signal_set, &s); cout << "not now..." << endl;}
        
        
        cout << "2[" << my_signal << "==" << s << "] " << end_is_reached << " " << buff << " " << endl;
        
        if (!read(read_from_pipe_des, &buff, 1) && end_is_reached)
        {
            cout << "[Pipe is empty and writer is dead. Terminating...]" << endl;
            close(read_from_pipe_des);
            break;
        }
        else if (!end_is_reached) {
            for (int i = 0; read(read_from_pipe_des, &buff, 1) == 0 && i < 5; i++)
                usleep(100000);
        }
        
        if(buff == '\0') break;
        
        output_file << buff;
        cout << buff << endl;
        kill(0, sibling_signal);
        
        
    }
    
    kill(0, sibling_signal);
    output_file.close();
    cout << endl << endl << "CLOSING TIME!" << endl;
    
    exit(EXIT_SUCCESS);
}

#include <iostream>
#include <stdio.h>      // getc, FILE
#include <sys/unistd.h>     // pipe
#include <fstream>
#include <string>
#include <sys/signal.h>
#include <sys/wait.h>
using namespace std;

char if1_n[] = "input.txt";
char of1_n[] = "output1.txt";
char of2_n[] = "output2.txt";

char sub_prog_name[] = "./sub_prog";

// void do_so(int i) {
//     if(i == SIGUSR1)
//         kill(pid2, SIGUSR1);
//     else
//         kill(pid1, SIGUSR1);
// }

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
    
    
    if((pid1 = fork()) == 0)
        execl(sub_prog_name, sub_prog_name, &filedes[0], of1_n, "e", NULL);
    else if(pid1 < 0) { cout << "[Unexpected error]" << endl; return -1; }
    
    if((pid2 = fork()) == 0)
        execl(sub_prog_name, sub_prog_name, &filedes[0], of2_n, "o", NULL);
    else if(pid2 < 0) { cout << "[Unexpected error]" << endl; return -1; }
    
    close(filedes[0]);
    cout << "fsadf" << endl;
    
    
     
    // wait for inition (sucessors are ready to get signals)
    for(int i = 0; i < 2; i++) {
        sigwait(&signals, &s);
        cout << "MAIN GOT " << s << " " << pid1 << " " << pid2 << endl;
    }
    
//     signal(SIGUSR1, do_so);
//     signal(SIGUSR2, do_so);
    
    sigprocmask(SIG_UNBLOCK, &signals, NULL);
    kill(pid1, SIGUSR1);
    cout << "[PID1: SIGNAL SEND]" << endl;
    kill(pid2, SIGUSR2);
    cout << "[PID2: SIGNAL SEND]" << endl;
    /*
    for(int i = 0; !file_to_send.eof(); i++) {
        getline(file_to_send, buff);
        size_t sz = sizeof(buff);
        write(filedes[1], buff.c_str(), sz);
        
        
//         if(i % 2 == 0) kill(pid1, SIGUSR1);
//         else kill(pid2, SIGUSR2);
    }
    
    cout << endl << endl << "[Waiting for successors...]" << endl;
    kill(pid1, SIGQUIT);
    kill(pid2, SIGQUIT);
    */
    
    waitpid(pid1, &s, 0);
    waitpid(pid2, &s, 0);
    cout << endl << endl << "[Terminating...]" << endl;
    close(filedes[1]);
    file_to_send.close();
    
    return 0;
}

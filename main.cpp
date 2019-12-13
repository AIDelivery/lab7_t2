#include <iostream>
#include <unistd.h>     // pipe
#include <fstream>
#include <string>
#include <signal.h>
#include <wait.h>
#include <fcntl.h>
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
//     fcntl(filedes[0], F_SETFL, O_RDONLY);
//     fcntl(filedes[1], F_SETFL, O_WRONLY);
    
    
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigaddset(&signals, SIGUSR2);
    sigaddset(&signals, SIGQUIT);
    sigprocmask(SIG_BLOCK, &signals, NULL);
//     signal(SIGUSR1, do_so);
//     signal(SIGUSR2, do_so);
    
    
    if((pid1 = fork()) == 0)
        execl(sub_prog_name, sub_prog_name, &filedes[0], of1_n, "e", NULL);
    else if(pid1 < 0) { cout << "[Unexpected error]" << endl; return -1; }
    
    sigwait(&signals, &s); // getting 10
    
    if((pid2 = fork()) == 0)
        execl(sub_prog_name, sub_prog_name, &filedes[0], of2_n, "o", NULL);
    else if(pid2 < 0) { cout << "[Unexpected error]" << endl; return -1; }
    
    sigwait(&signals, &s); // getting 12
    
    close(filedes[0]); // closing "read-end"
    
    kill(pid1, SIGUSR1);    // init
    
    for(int i = 0; !file_to_send.eof(); i++) {
        getline(file_to_send, buff);
        size_t sz = buff.size();
        write(filedes[1], buff.c_str(), sz);
    }
    char t = '\0';
    write(filedes[1], &t, 1);
    write(filedes[1], &t, 1);
    
    close(filedes[1]);
    kill(0, SIGQUIT);
    cout << endl << endl << "[Waiting for successors...]" << endl;
    
    waitpid(pid1, &s, 0);
    cout << "PID1 CLOSED" << endl;
    waitpid(pid2, &s, 0);
    cout << "PID2 CLOSED" << endl;
    
    
    cout << endl << endl << "[Terminating...]" << endl;
    file_to_send.close();
    
    return 0;
}

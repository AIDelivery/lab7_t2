#include <iostream>
#include <stdio.h>      // getc, FILE
#include <unistd.h>     // pipe
#include <fstream>
#include <string>
#include <signal.h>
using namespace std;

char if1_n[] = "input.txt";
char of1_n[] = "output1.txt";
char of2_n[] = "output2.txt";

char sub_prog_name[] = "./sub_prog";

int main(int argc, char* argv[]) {
    ifstream file_to_send;
    pid_t pid1, pid2;
    int filedes[2];
    string buff;
    
    file_to_send.open(if1_n, std::ifstream::in);
    pipe(filedes);
    
    
    if((pid1 = fork()) == 0)
        execl(sub_prog_name, sub_prog_name, &filedes[0], of1_n, NULL);
    else if(pid1 < 0) { cout << "[Unexpected error]" << endl; return -1; }
    
    if((pid2 = fork()) == 0)
        execl(sub_prog_name, sub_prog_name, &filedes[0], of2_n, NULL);
    else if(pid2 < 0) { cout << "[Unexpected error]" << endl; return -1; }
    
    kill(pid1, SIGUSR1);
    kill(pid2, SIGUSR2);
    close(filedes[0]);
    
    
    for(int i = 0; !file_to_send.eof(); i++) {
        getline(file_to_send, buff);
        size_t sz = sizeof(buff);
        write(filedes[1], buff.c_str(), sz);
        
        
        if(i % 2 == 0) kill(pid1, SIGUSR1);
        else kill(pid2, SIGUSR2);
    }
    
    kill(pid1, SIGQUIT);
    kill(pid2, SIGQUIT);
    
    cout << endl << endl << "[Terminating...]" << endl;
    close(filedes[1]);
    file_to_send.close();
    
    return 0;
}

#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

pid_t child;
bool child_killed = false;

// parent
void send_sigusr1()
{
    kill(child, SIGUSR1);
}

void handle_sigint()
{
    if (child_killed) {
        puts("Goodbye!");
        exit(0);
    } else {
        kill(child, SIGUSR2);
    }
}

void handle_sigchld()
{
    child_killed = true;
}

void handle_timeout()
{
    kill(child, SIGTERM);
    exit(0);
}

// child
void handle_sigusr1()
{
    puts("Signal Received!");
}

void handle_sigusr2()
{
    puts("Child exiting...");
    exit(0);
}

int main()
{
    if ((child = fork()) < 0) {
        perror("fork");
        return 1;
    } else if (child > 0) { // parent
        signal(SIGTSTP, send_sigusr1);
        signal(SIGINT, handle_sigint);
        signal(SIGCHLD, handle_sigchld);
        signal(SIGALRM, handle_timeout);
        alarm(10);
        while (true)
            pause();
    } else { // child
        signal(SIGTSTP, SIG_IGN);        
        signal(SIGINT, SIG_IGN);        
        signal(SIGUSR1, handle_sigusr1);
        signal(SIGUSR2, handle_sigusr2);
        while (true)
            pause();
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#define FAILURE -1

int
command(const char* string, char *outbuf, int outlen, char *errbuf, int errlen){
    int stdoutPfds[2];
    int stderrPfds[2];
    pid_t pid;
    char *shellPath = "/bin/sh";
    int nBytes, status;

    sigset_t newmask, oldmask;
    struct sigaction ignore, saveint, savequit;

    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags = 0;

    if (string == NULL)
        return 1;

    if (sigaction(SIGINT, &ignore, &saveint) < 0){
        perror("SIGINT ignore error");
        return FAILURE;
    }
    if (sigaction(SIGQUIT, &ignore, &savequit) < 0){
        perror("SIGQUIT ignore error");
        return FAILURE;
    }

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
        perror("SIG_BLOCK error");
        exit(EXIT_FAILURE);
        return FAILURE;
    }

    if(pipe(stdoutPfds) == -1) {
        perror("pipe");
        return FAILURE;
    }

    if(pipe(stderrPfds) == -1) {
        perror("pipe");
        return FAILURE;
    }


    switch(pid = fork()) {
        case -1:
            perror("fork");
            return FAILURE;

        case 0: // Child
            close(1);
            if(dup(stdoutPfds[1]) == -1){// make stdout point to stdeoutPfds[1]
                perror("dup error");
                return FAILURE;
            }            

            close(2);
            if(dup(stderrPfds[1]) == -1){// make stderr point to stdeerrPfds[1]
                perror("dup error");
                return FAILURE;
            }

            close(stdoutPfds[0]); // read not required at child
            close(stderrPfds[0]); // read not required at child

            if (sigaction(SIGINT, &saveint, NULL) < 0) {
                perror("SIG_SETMASK error");
                return FAILURE;
            }
            if (sigaction(SIGQUIT, &savequit, NULL) < 0) {
                perror("SIG_SETQUIT error");
                return FAILURE;
            }
            if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
                perror("SIG_SETMASK error");
                return FAILURE;
            }
            if(execl(shellPath, "sh", "-c", string, (char *)0) < 0) {
                perror("execl error");
                return FAILURE;
            }
            _exit(127);

        default: // Parent
            close(stdoutPfds[1]); // write not required at parent
            close(stderrPfds[1]); // write not required at parent

            if (waitpid(pid, &status, 0) < 0){
                if (errno != EINTR) {
                    status = -1;
                    break;
                }
            }

            if((nBytes = read(stdoutPfds[0], outbuf, outlen)) < 0){
                perror("outbuf read error");
                return FAILURE;
            }
            outbuf[nBytes-1] = '\0';

            if((nBytes = read(stderrPfds[0], errbuf, errlen)) < 0){
                perror("errbuf read error");
                return FAILURE;
            }
            outbuf[nBytes-1] = '\0';
    }

    if (sigaction(SIGINT, &saveint, NULL) < 0) {
        perror("SIG_SETMASK error");
        return FAILURE;
    }
    if (sigaction(SIGQUIT, &savequit, NULL) < 0) {
        perror("SIG_SETQUIT error");
        return FAILURE;
    }
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        perror("SIG_SETMASK error");
        return FAILURE;
    }

    return status;
}

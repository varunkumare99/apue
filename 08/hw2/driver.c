#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int command(const char* string, char *outbuf, int outlen, char *errbuf, int errlen);
int
main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    char out[BUFSIZ], err[BUFSIZ];
    int val ;
    if ((val = command("ls -l .." , out, BUFSIZ-1, err, BUFSIZ-1)) == -1) {
        perror("error command");
        exit(EXIT_FAILURE);
    }
    out[BUFSIZ-1] = '\0';
    err[BUFSIZ-1] = '\0';
    printf("stdout:\n%s\n", out);
    printf("stderr:\n%s\n", err);
    return WEXITSTATUS(val);
}

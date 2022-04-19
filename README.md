# Advanced Programming in the UNIX Environment
Course [CS631 - APUE](https://stevens.netmeister.org/631/)
## week-1
* simple-cat.c : cat command implementation unbuffered I/O
* simple-cat2.c : cat command implementation with buffered I/0
* simple-ls.c: ls command implementation with no arguments
* uidGid.c: printing user and group id's
* simple-shell.c : basic shell implementation
* simple-shell2.c : basic shell implementation with SIGINT handler

## week-2
* openmax.c : determine number of open files a process can have using sysconf and getrlimit
* openex.c : illustrate opening of files
* lseek.c : check if a stdin is seekable.
* redir.c : output redirection via dup2.
* rwex.c : program to illustrate append.
* sync-cat.c : illustrate use of SYNC flag for immediate writes to disk.
* hole.c: it creates a hole in a file(sparse file).
* warmupExercise.c : find values of stdin, stdout, stderr.

##### homework-1
* hw1/bbcp: implemention of the barebones copy command.
* hw1/testcp.sh : test scipt for bbcp.
* hw1/bbcp.pdf : bbcp specification

## week-3
* access.c - check if user has access to given file.
* chmod.c - change permissions on file.
* chown.c - change ownership of a file.
* setuid.c - raise/drop user privileges.
* size.c - print file size, block size of the given file.
* umask.c - setting default permissions with umask.

##### midTerm Project (Implementation of ls(1) command in NETBSD)
* midProject/ls.pdf - project specification.
* midProject/src - source files

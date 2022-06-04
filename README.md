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
* midProject/src, midProject/include - source files, header files.
* midProject/Makefile - build the project.(ls binary file is created)
* midProject/ls-test - test script.
* midProject/makelstest - test scrips.

## week-4
* cd.c - to change current working directory.
* getpw.c - print userName/uid and iterate over all entries in /etc/passwd.
* groups.c - print groupName/gid and iterate over all group in system(/etc/groups).
* lns.c - create a soft link.
* rename.c - renaming a file (mv command).
* time.c - printing time in various formats.
* wait-unlink.c - to unlink a file. The space allocated to the file is cleared, once all the links pointing to the inode are deleted.

## week-5
* compileChain - compiling files upto various stages(preprocessing, compile, assembly, linking).
* gdb-examples - debugging errors with gdb.
* make-examples - automating build with Make.

## week-6
* memory-layout0.c - print the memory location of variable.
* memory-layout1.c - print the memory location of private variables, global variable and functions.
* memory-layout2.c - print the memory location of stack, heap, initialized data, unintialized data, text segment.
* memory-layout3.c - overflow the stack by by repeatedly calling function.
* memory-layout4.c - printing the memory of location environment variables.
* memory-layout5.c - accessing the last and first environment variable.
* memory-layout6.c - examining memory layout after updating a env variable to a larger value.
* memory-layout7.c - examining memory layout after addition of a new env variable.
* memory-layout8.c - examining memory layout after unsetting an env variable.
* entry1.c - print the address of function main.
* entry2.c - start the program from a different function apart from main.
* entry3.c - start the program from a different function and exit instead of return.
* entry4.c - comparing return statements between c89 and c11 standards.
* exit-handlers.c - terminaing a program using exit(),___ _exit(), abort().
* malloc.c - using malloc() and realloc();
* pid.c - get the process of current and the parent process.
* forkseek.c - illustrates sharing of file descriptors between parent and child processes.
* forkflush.c - illustrates sharing of buffers in between parent and child processes.
* zombies.c - illustrate creation of zombie process.

## week-7
* sigusr.c - illustrates the use of signals handlers, by implementing signal handlers for SIGUSR1 and SIGUSR2
* signals1.c - illustrates interuption while executing a signal handler.
* signals2.c - illustrates the default behaviour of signals handlers.
* signals3.c - illustrates example of sigaction.
* signals4.c - illustrates the risk of buffered I/O from within the signal handler.
* reentrant.c - illustrates the risk of calling non-re-entrant functions from within the signal handler.
* eintr.c - illustrates the restart of a system call from being interupted.

## week-8
* semdemo.c - illustrates the use of semaphores.
* shemdemo.c - illustrates the use of shared memory.
* msgrecv.c, msgsend.c - illustrates the use of message queues.
* mqrecv.c, mqsend.c - illustrates the use of POSIX message queues.
* pipe1.c - illustsrates echo msg | cat pipeline. 
* pipe2.c - illustrates cat file.txt | ${PAGER:-/usr/bin/more}
* popen.c - same as pipe2.c but using popen(3).

##### homework-2
* hw2/command.c : pass a command to the shell and return stdout and stderr.
* hw2/driver.c : example driver program to run command.
* hw2/command.pdf : command specification
The program uses the following:
- create two pipes, one for stdout, one for stderr of the command to execute
- ignore SIGINT, SIGQUIT; block SIGCHLD
- fork(2) a new process
- dup2(2) the respective file descriptors onto the write-ends of the pipes
- exec(3) the command in question
- read(2) from the read-ends of the pipes
- append output to the respective buffer, careful to not overflow them
- wait(3) for the process to terminate


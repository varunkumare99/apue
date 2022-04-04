/*
 * bbcp - bare bones copy of a file
 * bbcp source target
 * The bbcp utility copies the contents of the source to target.
 * If target is a directory, bbcp will copy source into this directory.
 * bbcp exits 0 on success, and >0 if an error occurred
 */

#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <libgen.h>

#define BUFFSIZE 16384 
#define FILENAMESIZE 100
#define TRUE 1
#define FALSE 0

/*
 * check if given path is a directory
 */
int 
isDirectory(const char* path){
    struct stat path_stat;
    stat(path,&path_stat);
    return S_ISDIR(path_stat.st_mode);
}

/*
 * check if path exists
 */
int
doesPathExist(const char* path){
    struct stat path_stat;
    if(stat(path, &path_stat) == 0){
        return TRUE;
    }
    return FALSE;
}

/* 
 * check path points to a regular file or FIFO
 */
int 
isRegFileOrFifo(const char* path){
    struct stat path_stat;
    stat(path,&path_stat);
    return S_ISREG(path_stat.st_mode) || S_ISFIFO(path_stat.st_mode);
}

/*
 * check if f1 and f2 are hardlinks. We identify this by checking
 * if the point to the same inode
 */
int
isHardLinks(char* file1, char* file2){
    struct stat file1_stat;
    struct stat file2_stat;

    stat(file1, &file1_stat);
    stat(file2, &file2_stat);

    if(file1_stat.st_ino == file2_stat.st_ino)
        return TRUE;
    else
        return FALSE;
}

/*
 * check if source is invalid. 
 * if path exists check if source is other than regular file or fifo
 */
int
isSourceNotValid(const char* source){
    return !doesPathExist(source) || !isRegFileOrFifo(source);
}

/*
 * check if the given target is a valid directory
 */
int
isTargetDir(const char* target){
    return doesPathExist(target) && isDirectory(target);
}

/*
 * check if the target file already exists
 */
int
doesTargetFileExist(const char* target){
    return doesPathExist(target) && isRegFileOrFifo(target);
}

/*
 * check if the target point to current directory
 */
int
isTargetCurrentDir(const char* target){
    return (strcmp(target,".") == 0);
}

/*
 * check if source and target are the same file
 */
int
isSourceAndTargetSame(const char* source, const char* target){
    return (strcmp(source, target) == 0);
}

/*
 * generate valid target name, using dirPath and fileName.
 */
char*
generateTargetName(char* dirPath, char* fileName){
    char *targetName;
    targetName = (char*) malloc(FILENAMESIZE);
    if(dirPath[strlen(dirPath)-1] == '/'){
        strcat(targetName, dirPath);
        strcat(targetName, fileName);
    }
    else{
        strcat(targetName, dirPath);
        strcat(targetName,"/");
        strcat(targetName, fileName);
    }
    return (char*)targetName;
}

/*
 * bbcopy read the contents from readfd file descriptor
 * and write the contents to targentName file
 */
void
bbcopy(int* readfd, char* targetName){
    int n, writefd;
    char buf[BUFFSIZE];

    if ((writefd = open(targetName, O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR)) == -1) {
        fprintf(stderr, "bbcp error: Unable to open %s: %s\n",targetName,strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((n = read(*readfd, buf, BUFFSIZE)) > 0) {
        if (write(writefd, buf, n) != n) {
            fprintf(stderr, "bbcp error: Unable to write to file %s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    (void)close(*readfd);
    (void)close(writefd);
}


int
main(int agrv, char** argc) {

    if(agrv != 3){
        fprintf(stderr, "bbcp error: command takes two arguments source and target\n");
        exit(EXIT_FAILURE);
    }

    int readfd;
    if(isSourceNotValid(argc[1])){
        fprintf(stderr, "bbcp error: source %s, not valid \n",argc[1]);
        exit(EXIT_FAILURE);
    }

    if ((readfd = open(argc[1], O_RDONLY, S_IRUSR | S_IWUSR)) == -1) {
        fprintf(stderr, "bbcp error: Unable to open %s: %s\n",argc[1],strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(isTargetDir(argc[2])){
        char *sourceName = basename(argc[1]);
        char* targetName = generateTargetName(argc[2],sourceName);
        if(isTargetCurrentDir(argc[2])){
            char* onlyTargetName = basename(targetName);
            if(isSourceAndTargetSame(argc[1], onlyTargetName)){
                fprintf(stderr, "bbcp error: %s and ./%s are the same file \n",argc[1],argc[1]);
                exit(EXIT_FAILURE);
            }
        }
        bbcopy(&readfd,targetName);
    }

    else if(doesTargetFileExist(argc[2])){
        if(isHardLinks(argc[1],argc[2])){
            fprintf(stderr,"bbcp error:  %s and %s point to the same inode, they are hardlinks\n", argc[1], argc[2]);
            exit(EXIT_FAILURE);
        }
        else{
            bbcopy(&readfd, argc[2]);
        }
    }

    else{
        bbcopy(&readfd, argc[2]);
    }
    return EXIT_SUCCESS;
}

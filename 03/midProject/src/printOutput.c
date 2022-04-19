#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fts.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>


#include "../include/compare.h"
#include "../include/flags.h"

#define TIMEBUFFSIZE 20

static void
printFileNameByChar(const char* name){
    for(int i=0; i<strlen(name); ++i){
        if(f_qflag){
            if(isprint(name[i])){
                putchar(name[i]);
            }
            else{
                putchar('?');
            }
        }
        else{
            putchar(name[i]);
        }
    }
}

static void
printSymLink(const char* path,const char* name){
    char reference[PATH_MAX];
    char fullName[PATH_MAX];

    fullName[0] = '\0';
    reference[0] = '\0';

    (void)strncat(fullName,path,strlen(path));
    if(f_Rflag){
        (void)strncat(fullName,name,strlen(name));
    }

    int n;
    if((n = readlink(fullName,reference,PATH_MAX-1)) == -1){
        fprintf(stderr,"ls: readLink failed: %s, %s\n", fullName, strerror(errno));
        return;
    }
    reference[n]='\0';
    if(f_Fflag){
        printf("@ -> %s",reference);
    }
    else{
        printf(" -> %s",reference);
    }

}

static void 
printFileName(mode_t st_mode,const char* path,const char* name){
    printFileNameByChar(name);

    if(!f_Fflag && (S_ISLNK(st_mode))){
        printSymLink(path,name);
    }

    if(f_Fflag){
        if(S_ISDIR(st_mode)){
            printf("/");
        }
        else if(S_ISLNK(st_mode)){
            printSymLink(path,name);
        }
        else if(S_ISSOCK(st_mode)){
            printf("=");
        }
        else if(S_ISFIFO(st_mode)){
            printf("|");
        }
        else if(S_ISWHT(st_mode)){
            printf("%");
        }
        else if((st_mode & S_IXUSR) || (st_mode & S_IXGRP) || (st_mode & S_IXOTH)){
            printf("*");
        }
    }
}

static void
printLinks(nlink_t st_nlink){
    printf("%d",st_nlink);
}

static void 
printOwnerName(uid_t st_uid){
    if(f_nflag){
        printf("%-*d",w_ownerNameWidth,st_uid);
    }
    else{
        struct passwd* pw;
        if((pw = getpwuid(st_uid)) == NULL){
            printf("%-*d",w_ownerNameWidth,st_uid);
        }
        else{
            printf("%-*s",w_ownerNameWidth,pw->pw_name);
        }
    }
}

static void 
printTime(time_t t){
    char timeBuf[TIMEBUFFSIZE] ;
    time_t now = time(NULL);
    struct tm tmt, tmNow;

    if(localtime_r(&t, &tmt) == NULL){
        fprintf(stderr,"ls: localTime conversion error :  %s\n", strerror(errno));
    }
    if(localtime_r(&now, &tmNow) == NULL){
        fprintf(stderr,"ls: localTime conversion error :  %s\n", strerror(errno));
    }

    if(tmt.tm_year == tmNow.tm_year){
        strftime(timeBuf,TIMEBUFFSIZE,"%b %e %R",&tmt);
    }
    else{
        strftime(timeBuf,TIMEBUFFSIZE,"%b %e %Y",&tmt);
    }
    printf("%s",timeBuf);
}

static void 
printGroupName(gid_t st_gid){
    if(f_nflag){
        printf("%-*d",w_groupNameWidth,st_gid);
    }
    else{
        struct group* gp ;
        if((gp = getgrgid(st_gid)) == NULL){
            printf("%-*d",w_groupNameWidth,st_gid);
        }
        else{
            printf("%-*s",w_groupNameWidth,gp->gr_name);
        }
    }
}

static void
printFileType(mode_t st_mode) {
    switch((st_mode & S_IFMT)){
        case S_IFREG:
            printf("-");
            break;
        case S_IFDIR:
            printf("d");
            break;
        case S_IFCHR:
            printf("c");
            break;
        case S_IFBLK:
            printf("b");
            break;
        case S_IFIFO:
            printf("p");
            break;
        case S_IFLNK:
            printf("l");
            break;
        case S_IFSOCK:
            printf("s");
            break;
        case S_IFWHT:
            printf("w");
            break;
        case S_ARCH1:
            printf("a");
            break;
        case S_ARCH2:
            printf("A");
            break;
        default:
            fprintf(stderr,"ls: unrecognized file type : %lu\n",st_mode);
    }
}

static void 
printFilePermissions(mode_t st_mode){

    (st_mode & S_IRUSR)? printf("r"):printf("-");
    (st_mode & S_IWUSR)?printf("w"):printf("-");

    if((st_mode & S_IXUSR) && (st_mode & S_ISUID)){
        printf("s");
    }
    else if(st_mode & S_IXUSR){
        printf("x");
    }
    else if(st_mode & S_ISUID){
        printf("S");
    }
    else{
        printf("-");
    }

    (st_mode & S_IRGRP)? printf("r"):printf("-");
    (st_mode & S_IWGRP)? printf("w"):printf("-");

    if((st_mode & S_IXGRP) && (st_mode & S_ISGID)){
        printf("s");
    }
    else if(st_mode & S_IXGRP){
        printf("x");
    }
    else if(st_mode & S_ISGID){
        printf("S");
    }
    else{
        printf("-");
    }
    (st_mode & S_IROTH)?printf("r"):printf("-");
    (st_mode & S_IWOTH)?printf("w"):printf("-");
    if((st_mode & S_IXOTH) && (st_mode & S_ISVTX)){
        printf("t");
    }
    else if(st_mode & S_IXOTH){
        printf("x");
    }
    else if(st_mode & S_ISVTX){
        printf("T");
    }
    else{
        printf("-");
    }
}

static void 
printFileSize(off_t st_size){
    if(f_hflag){
        char output[5];
        if(humanize_number(output,sizeof(output),st_size,"",HN_AUTOSCALE,(HN_DECIMAL | HN_NOSPACE | HN_B)) == -1){
            fprintf(stderr,"ls: failed to humaize %lu\n",st_size);
        }
        printf("%*s",w_fileSizeWidth,output);
    }
    else{
        printf("%*lu",w_fileSizeWidth,st_size);
    }
}

static void
printBlocks(long st_blocks){
    const char* blockSize = getenv("BLOCKSIZE");
    long int lBlockSize;
    char *ptr;

    if(f_kflag){
        printf("%*lu",w_blockSizeWidth, st_blocks/2);
    }
    else if(f_hflag){
        char output[5];
        if(humanize_number(output,sizeof(output),st_blocks*512,"",HN_AUTOSCALE,(HN_DECIMAL | HN_NOSPACE | HN_B)) == -1){
            fprintf(stderr,"ls: failed to humaize %lu\n",st_blocks);
        }
        printf("%*s",w_blockSizeWidth,output);
    }
    else if(blockSize != NULL){
        lBlockSize = strtol(blockSize, &ptr, 10);
        if(lBlockSize > 512){
            lBlockSize /= 512;
            printf("%*lu",w_blockSizeWidth,st_blocks/lBlockSize + (st_blocks%lBlockSize != 0));
        }
        else{
            printf("%*lu",w_blockSizeWidth,st_blocks);
        }
    }
    else{
        printf("%*lu",w_blockSizeWidth,st_blocks);
    }
}

static void 
printInode(ino_t st_ino){
    printf("%lu",st_ino);
}

static void
printMajorMinorNumber(dev_t st_rdev){
    printf("%d,%d",major(st_rdev),minor(st_rdev));
}

static int
isDevice(mode_t st_mode){
    return (S_ISCHR(st_mode) || S_ISBLK(st_mode));
}     

void printData(const FTSENT* node, const char* path){
    const struct stat* sb = node->fts_statp;
    if(f_iflag){
        printInode(sb->st_ino);
        printf(" ");
    }

    if(f_sflag){
        printBlocks(sb->st_blocks);
        printf(" ");
    }

    if(f_lflag){
        printFileType(sb->st_mode);
        printFilePermissions(sb->st_mode);
        printf(" ");
        printLinks(sb->st_nlink);
        printf(" ");
        printOwnerName(sb->st_uid);
        printf(" ");

        printGroupName(sb->st_gid);
        printf(" ");
        if(isDevice(sb->st_mode)){
            printMajorMinorNumber(sb->st_rdev);
            printf(" ");
        }
        printFileSize(sb->st_size);
        printf(" ");
        printTime(sb->st_mtime);
        printf(" ");
    }
    if(f_dflag){
        printFileNameByChar(path);
    }
    else{
        printFileName(sb->st_mode,node->fts_path,node->fts_name);
    }
    printf("\n");
}

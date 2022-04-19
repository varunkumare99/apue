#include <stdlib.h>
#include <unistd.h>
#include <fts.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <ctype.h>
#include <limits.h>
#include "../include/compare.h"

#define BLOCKSIZE 512

int f_Aflag;         /* List all entries except for '.' and '..' */
int f_aflag;         /* Include directory entries whose name begins with a '.' */
int f_cflag;         /* use time when file status was last changed */
int f_dflag;         /* Directories are listed as plain files(not searched Recursively) */
int f_Fflag;         /* display '/' after dir, '*' after exe, '%' after whiteout, '@' after symlink, '=' after socket, '|' after pipi */
int f_fflag;         /* output is not sorted */
int f_hflag;         /* display sized in human readable format */
int f_iflag;         /* for each file, print it's inode */
int f_kflag;         /* report sizes in terms of kilo bytes*/
int f_lflag;         /* display in long format */
int f_nflag;         /* same as -l, except owner and group IDs are displayed numerically */
int f_qflag;         /* Force non printable characters in filenames as '?' */
int f_Rflag;         /* Recursively list subdirectories encountered */
int f_rflag;         /* reverse the order of the sort */
int f_Sflag;         /* sort by size, largest file first*/
int f_sflag;         /* Display the number of blocks used by each file */
int f_tflag;         /* sort by time modified */
int f_uflag;         /* sort by time access time */
int f_wflag;         /* force raw printing of non printable characters */

int w_ownerNameWidth;     /* owner Name column width */ 
int w_groupNameWidth;     /* group Name column width */
int w_fileSizeWidth;      /* file Size column width */
int w_blockSizeWidth;     /* block size column width */


int (*sortCompare)(const FTSENT** one, const FTSENT** two);
void printData(const FTSENT* node, const char* path);
char* const* sourceFile;

static void
initWidths(){
    w_ownerNameWidth = 0;
    w_groupNameWidth = 0;
    w_fileSizeWidth = 0;
    w_blockSizeWidth = 0;
}

static int
getNumOfDigits(off_t st_size)
{
    int count = 0;
    while (st_size != 0)
    {
        st_size = st_size / 10;
        ++count;
    }
    return count;
}

/*convert size into human readable size */
static int 
getHumanReadableDigits(off_t st_size){
    char output[5];
    int n;
    if((n = humanize_number(output,sizeof(output),st_size,"",HN_AUTOSCALE,(HN_DECIMAL | HN_NOSPACE | HN_B))) == -1){
        fprintf(stderr,"ls: failed to humaize %lu",st_size);
    }
    return n;
}

static void
handleBlockSizeErrors(long int lBlockSize, const char* blockSize, char *ptr){
    if((ptr == blockSize) ||
            (*ptr != '\0') ||
            (errno == ERANGE) ||
            (lBlockSize < INT_MIN ) ||
            (lBlockSize > INT_MAX)){
        fprintf(stderr,"ls: invalid blockSize  %s, %s\n",blockSize, strerror(errno));
    }
}

/*print the total number of blocks used by a directory */
static void
printDirBlocks(long totalSize){
    const char* blockSize = getenv("BLOCKSIZE");
    long int lBlockSize;
    char *ptr;
    if(f_hflag){
        char output[5];
        if(humanize_number(output,sizeof(output),totalSize*BLOCKSIZE,"",HN_AUTOSCALE,(HN_DECIMAL | HN_NOSPACE | HN_B)) == -1){
            fprintf(stderr,"ls: failed to humaize %lu",totalSize);
        }
        printf("total %s\n",output);

    }
    else if(f_kflag){
        printf("total %ld\n",totalSize/2);
    }
    else if(blockSize != NULL){
        errno = 0;
        lBlockSize = strtol(blockSize, &ptr, 10);
        handleBlockSizeErrors(lBlockSize, blockSize, ptr);
        if(lBlockSize > 512){
            lBlockSize /= 512;
            totalSize = totalSize/lBlockSize + (totalSize % lBlockSize != 0);
            printf("total %ld\n",totalSize);
        }
        else{
            printf("total %ld\n",totalSize);
        }
    }
    else{
        printf("total %ld\n",totalSize);
    }
}

static void 
printTotalDirBlockSize(const FTSENT* node){
    long totalSize = 0;
    if(!f_Rflag){
        /* Since, the recurive flag is not set we evalute on at the parent level */

        FTS* file_system = NULL;
        if((file_system = fts_open(sourceFile, FTS_COMFOLLOW | FTS_NOCHDIR | FTS_SEEDOT | FTS_PHYSICAL, sortCompare)) == NULL){
            fprintf(stderr,"ls: source: %s , %s\n",*sourceFile, strerror(errno));
            exit(EXIT_FAILURE);
        }

        while( (node = fts_read(file_system)) != NULL){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }

            /* read files only at level 1, since R flag is not set */
            if((node->fts_level == 1) && (node->fts_info != FTS_DP)){
                totalSize += (node->fts_statp)->st_blocks;
            }
            node = node->fts_link;
        }
        printDirBlocks(totalSize); 
    }
    else{
        /* recursive mode, evaulate children */

        while(NULL != node){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }
            totalSize += (node->fts_statp)->st_blocks;
            node = node->fts_link;
        }
        printDirBlocks(totalSize); 
    }
}

static void
setMaxOwnerNameWidthForNode(const FTSENT* node){
    int length;
    const struct stat* sb;
    struct passwd* pw;
    sb = node->fts_statp; 
    if(f_nflag){
        length = getNumOfDigits(sb->st_uid);
    }
    else{
        if((pw = getpwuid(sb->st_uid)) == NULL){
            length = getNumOfDigits(sb->st_uid);
        }
        else{
            length = strlen(pw->pw_name);
        }
    }
    if(w_ownerNameWidth < length){
        w_ownerNameWidth = length;
    }
}

/* set the Maximum column name width for owner name */
static void
setMaxOwnerNameWidth(const FTSENT* node){
    if(!f_Rflag){
        /* Since, the recurive flag is not set we evalute on at the parent level */

        FTS* file_system = NULL;
        if((file_system = fts_open(sourceFile, FTS_COMFOLLOW | FTS_NOCHDIR | FTS_SEEDOT, sortCompare)) == NULL){
            fprintf(stderr,"ls: source: %s , %s\n",*sourceFile, strerror(errno));
            exit(EXIT_FAILURE);
        }

        while( (node = fts_read(file_system)) != NULL){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }

            /* read files only at level 1, since R flag is not set */
            if((node->fts_level == 1) && (node->fts_info != FTS_DP)){
                setMaxOwnerNameWidthForNode(node);
            }
            node = node->fts_link;
        }
    }
    else{
        /* recursive mode, evaulate children */

        while(NULL != node){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }
            setMaxOwnerNameWidthForNode(node);
            node = node->fts_link;
        }
    }
}

static void
setMaxGroupNameWidthForNode(const FTSENT* node){
    int length;
    const struct stat* sb; 
    struct group* gp;
    sb = node->fts_statp;
    if(f_nflag){
        length = getNumOfDigits(sb->st_gid);
    }
    else{
        if((gp = getgrgid(sb->st_gid)) == NULL){
            length = getNumOfDigits(sb->st_gid);
        }
        else{
            length = strlen(gp->gr_name);
        }
    }
    if(w_groupNameWidth < length)
    {
        w_groupNameWidth = length;
    }
}

/* set the Maximum column name width for group name */
static void
setMaxGroupNameWidth(const FTSENT* node){
    if(!f_Rflag){
        /* Since, the recurive flag is not set we evalute on at the parent level */

        FTS* file_system = NULL;
        if((file_system = fts_open(sourceFile, FTS_COMFOLLOW | FTS_NOCHDIR | FTS_SEEDOT, sortCompare)) == NULL){
            fprintf(stderr,"ls: source: %s , %s\n",*sourceFile, strerror(errno));
            exit(EXIT_FAILURE);
        }
        while( (node = fts_read(file_system)) != NULL){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }

            /* read files only at level 1, since R flag is not set */
            if((node->fts_level == 1) && (node->fts_info != FTS_DP)){
                setMaxGroupNameWidthForNode(node);
            }
            node = node->fts_link;
        }
    }
    else{
        /* recursive mode, evaulate children */

        while(NULL != node){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }
            setMaxGroupNameWidthForNode(node);
            node = node->fts_link;
        }
    }
}

static void
setMaxFileSizeWidthForNode(const FTSENT* node){

    int length;
    const struct stat* sb;
    sb = node->fts_statp;
    if(f_hflag){
        length = getHumanReadableDigits(sb->st_size);
    }
    else{
        length = getNumOfDigits(sb->st_size);
    }
    if(w_fileSizeWidth < length){
        w_fileSizeWidth = length;
    }
}


/* set the Maximum column name width for file size */
static void
setMaxFileSizeWidth(const FTSENT* node){
    if(!f_Rflag){
        /* Since, the recurive flag is not set we evalute on at the parent level */

        FTS* file_system = NULL;
        if((file_system = fts_open(sourceFile, FTS_COMFOLLOW | FTS_NOCHDIR | FTS_SEEDOT, sortCompare)) == NULL){
            fprintf(stderr,"ls: source: %s , %s\n",*sourceFile, strerror(errno));
            exit(EXIT_FAILURE);
        }

        while( (node = fts_read(file_system)) != NULL){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }

            /* read files only at level 1, since R flag is not set */
            if((node->fts_level == 1) && (node->fts_info != FTS_DP)){
                setMaxFileSizeWidthForNode(node);
            }
            node = node->fts_link;
        }
    }
    else{
        /* recursive mode, evaulate children */

        while(NULL != node){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }
            setMaxFileSizeWidthForNode(node);
            node = node->fts_link;
        }
    }
}

static void
setMaxBlockSizeWidthForNode(const FTSENT* node){
    int length;
    const struct stat* sb;
    sb = node->fts_statp;
    if(f_hflag){
        length = getHumanReadableDigits(sb->st_blocks*BLOCKSIZE);
    }
    else if(f_kflag){
        length = getNumOfDigits(sb->st_blocks/2);
    }
    else{
        length = getNumOfDigits(sb->st_blocks);
    }
    if(w_blockSizeWidth < length){
        w_blockSizeWidth = length;
    }
}

/* set the Maximum column name width for block size */
static void
setMaxBlockSizeWidth(const FTSENT* node){
    if(!f_Rflag){
        /* Since, the recurive flag is not set we evalute on at the parent level*/

        FTS* file_system = NULL;
        if((file_system = fts_open(sourceFile, FTS_COMFOLLOW | FTS_NOCHDIR | FTS_SEEDOT, sortCompare)) == NULL){
            fprintf(stderr,"ls: source: %s , %s\n",*sourceFile, strerror(errno));
            exit(EXIT_FAILURE);
        }

        while( (node = fts_read(file_system)) != NULL){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }

            /* read files only at level 1, since R flag is not set */
            if((node->fts_level == 1) && (node->fts_info != FTS_DP)){
                setMaxBlockSizeWidthForNode(node);
            }
            node = node->fts_link;
        }
    }
    else{
        /* recursive mode, evaulate children */

        while(NULL != node){

            /* Ignore "." and ".." if A flag is set*/
            if((f_Aflag && node->fts_info == FTS_DOT)){
                node = node->fts_link;
                continue;
            }

            /* Ignore files the begin with "." if a flag is not set */
            if((!f_aflag && *(node->fts_name) == '.')){
                node = node->fts_link;
                continue;
            }
            setMaxBlockSizeWidthForNode(node);
            node = node->fts_link;
        }
    }
}

static int 
checkNodeError(const FTSENT* node){
    if(node->fts_info == FTS_DNR || node->fts_info == FTS_ERR || node->fts_info == FTS_NS){
        fprintf(stderr,"ls: source: %s: %s\n",node->fts_path, strerror(node->fts_errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static void
setColumnWidths(const FTSENT* node){
    setMaxOwnerNameWidth(node);
    setMaxGroupNameWidth(node);
    setMaxFileSizeWidth(node);
    setMaxBlockSizeWidth(node);
}

static int 
handleFiles(const FTSENT* node,FTS* file_system){
    if(node->fts_info == FTS_F){
        printData(node,node->fts_accpath);
        fts_close(file_system);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

static int
ls(char* const *source) {
    FTS* file_system = NULL;
    FTSENT* child = NULL;
    FTSENT* parent = NULL;
    FTSENT* temp = NULL;

    if((file_system = fts_open(source, FTS_COMFOLLOW | FTS_NOCHDIR | FTS_SEEDOT, sortCompare)) == NULL){
        fprintf(stderr,"ls: fts_open : source: %s , %s\n",*source, strerror(errno));
        exit(EXIT_FAILURE);
    }
    sourceFile = source;

    if(f_dflag){ 
        /* print only directory details */
        while( (parent = fts_read(file_system)) != NULL){
            if(!checkNodeError(parent)){
                if((parent->fts_level == 0) && (parent->fts_info != FTS_DP)){
                    printData(parent,parent->fts_accpath);
                }
            }
        }
    }
    else{
        /* Recursive (-R) or list all the files in the directory */
        while((parent = fts_read(file_system)) != NULL){
            if(f_Rflag){
                if(checkNodeError(parent)){
                    continue;
                }

                if((parent->fts_level == 0) && (parent->fts_info == FTS_F)){
                    printData(parent,parent->fts_accpath);
                    continue;
                }

                if(parent->fts_info == FTS_D){
                    child = fts_children(file_system,0);
                    if(errno != 0){
                        fprintf(stderr,"%s,ls: fts_children: %s\n",parent->fts_accpath,strerror(errno));
                        continue;
                    }

                    if(checkNodeError(child)){
                        continue;
                    }
                    if(NULL != child){
                        printf("\n%s:\n", child->fts_accpath);
                        printTotalDirBlockSize(child);
                        setColumnWidths(child);

                        while((NULL != child)) {
                            /* Ignore "." and ".." if A flag is set*/
                            if((f_Aflag && child->fts_info == FTS_DOT)){
                                child = child->fts_link;
                                continue;
                            }
                            /* Ignore files the begin with "." if a flag is not set */
                            if((!f_aflag && *(child->fts_name) == '.')){
                                child = child->fts_link;
                                continue;
                            }
                            printData(child,NULL);
                            child = child->fts_link;
                        }
                    }
                }
            }
            else{
                /* non - recursive mode */

                if(checkNodeError(parent)){
                    continue;
                }

                if((parent->fts_level == 0) && (parent->fts_info == FTS_D)){
                    printf("\n%s:\n", parent->fts_accpath);
                    printTotalDirBlockSize(parent);
                    setColumnWidths(parent);
                }

                if((parent->fts_level == 0) && (parent->fts_info == FTS_F)){
                    printData(parent,parent->fts_accpath);
                    continue;
                }

                /* Ignore "." and ".." if A flag is set*/
                if((f_Aflag && parent->fts_info == FTS_DOT)){
                    continue;
                }

                /* Ignore files the begin with "." if a flag is not set */
                if((!f_aflag && *(parent->fts_name) == '.')){
                    continue;
                }
                /* read files only at level 1, since R flag is not set */
                if((parent->fts_level == 1) && (parent->fts_info != FTS_DP)){
                    printData(parent,NULL);
                }
            }
        }
    }
    fts_close(file_system);
    return EXIT_SUCCESS;
}

static void
setSortFunction(){
    if(f_fflag){ 
        /* no sorting is applied */
        sortCompare = NULL;
    }
    if(f_rflag){
        if(f_cflag || f_tflag || f_uflag){
            /* reverse sort based on time */
            if(f_cflag){
                sortCompare = revFileStatusTimeCmp;
            }
            else if(f_tflag){
                sortCompare = revFileModifiedTimeCmp;
            }
            else{
                sortCompare = revFileAccessTimeCmp;
            }
        }
        else if(f_Sflag){
            /* reverse sort based on file size */
            sortCompare = revSizeCmp;
        }
        else{
            /* default reverse sort by file name in lexicographical order */
            sortCompare = revNameCmp;
        }
    }
    else{
        if(f_cflag || f_tflag || f_uflag){
            /* sort based on time */
            if(f_cflag){
                sortCompare = fileStatusTimeCmp;
            }
            else if(f_tflag){
                sortCompare = fileModifiedTimeCmp;
            }
            else{
                sortCompare = fileAccessTimeCmp;
            }

        }
        else if(f_Sflag){
            /* sort based on file size(largest first) */
            sortCompare = sizeCmp;
        }
        else{
            /* default sort by file name in lexicographical order*/
            sortCompare = nameCmp;
        }
    }
}

    static void
usage()
{
    fprintf(stderr,"usage: ls [-AacdFfhiklnqRrSstuw] [file ...]\n");
    exit(EXIT_FAILURE);
}

int
setFlags(int argc, char* const*argv){
    int index;
    int c;
    setprogname(argv[0]);
    if(!geteuid()){
        f_Aflag = 1;
    }
    opterr = 0;
    f_qflag = 1;
    while((c = getopt(argc,argv, "AacdFfhiklnqRrSstuw")) != -1){
        switch(c){
            case 'A':
                f_Aflag = 1;
                break;
            case 'a':
                f_aflag = 1;
                break;
            case 'c':
                f_cflag = 1; 
                f_uflag = 0;
                f_tflag = 0;
                break;
            case 'd':
                f_dflag = 1;
                break;
            case 'F':
                f_Fflag = 1;
                break;
            case 'f':
                f_fflag = 1;
                break;
            case 'h':
                f_hflag = 1; 
                f_kflag = 0;
                break;
            case 'i':
                f_iflag = 1;
                break;
            case 'k':
                f_kflag = 1;
                f_hflag = 0;
                break;
            case 'l':
                f_lflag = 1;
                break;
            case 'n':
                f_nflag = 1;
                break;
            case 'q':
                f_qflag = 1;
                f_wflag = 0;
                break;
            case 'R':
                if(!f_dflag)
                    f_Rflag = 1;
                break; 
            case 'r':
                f_rflag = 1;
                break;
            case 'S':
                f_Sflag = 1;
                break;
            case 's':
                f_sflag = 1;
                break;
            case 't':
                f_tflag = 1;
                f_uflag = 0;
                f_cflag = 0;
                break;
            case 'u':
                f_uflag = 1;
                f_cflag = 0;
                f_tflag = 0;
                break;
            case 'w':
                f_wflag = 1;
                f_qflag = 0;
                break;
            default:
            case '?':
                usage();
        }
    }

    setSortFunction();
    if(optind >= argc){
        char dotString[] = ".";
        char *dotargv[] = {dotString,NULL};
        ls(dotargv);
    }
    else{
        argv += optind;
        ls(argv);
    }

    return 0;
}


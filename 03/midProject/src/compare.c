#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

/* Comparison based on lexicographical order of names */
int
nameCmp(const FTSENT** one, const FTSENT** two){
    return (strcmp((*one)->fts_name, (*two)->fts_name));
}

/* Comparison based on reverse lexicographical order of names */
int
revNameCmp(const FTSENT** one, const FTSENT** two){
    return (strcmp((*two)->fts_name, (*one)->fts_name));
}

/* Comparison based on size of file(largest file first) */
int
sizeCmp(const FTSENT** one, const FTSENT** two){
    if(((*one)->fts_statp)->st_size > ((*two)->fts_statp)->st_size)
        return -1;
    else if(((*one)->fts_statp)->st_size < ((*two)->fts_statp)->st_size)
        return 1;
    else
        return 0;
}

/* Comparison based on size of file(smallest file first) */
int
revSizeCmp(const FTSENT** one, const FTSENT** two){
    if(((*one)->fts_statp)->st_size > ((*two)->fts_statp)->st_size)
        return 1;
    else if(((*one)->fts_statp)->st_size < ((*two)->fts_statp)->st_size)
        return -1;
    else
        return 0;
}

/* Comparison based on file status time(c_time) */
int
fileStatusTimeCmp(const FTSENT** one, const FTSENT** two){
    if(((*one)->fts_statp)->st_ctime > ((*two)->fts_statp)->st_ctime)
        return -1;
    else if(((*one)->fts_statp)->st_ctime < ((*two)->fts_statp)->st_ctime)
        return 1;
    else
        return nameCmp(one,two);
}

/* Comparison based on reverse file status time(c_time) */
int
revFileStatusTimeCmp(const FTSENT** one, const FTSENT** two){
    if(((*one)->fts_statp)->st_ctime > ((*two)->fts_statp)->st_ctime)
        return 1;
    else if(((*one)->fts_statp)->st_ctime < ((*two)->fts_statp)->st_ctime)
        return -1;
    else
        return revNameCmp(one,two);
}

/* Comparison based on file modified time (m_time) */
int
fileModifiedTimeCmp(const FTSENT** one, const FTSENT** two){
    if(((*one)->fts_statp)->st_mtime > ((*two)->fts_statp)->st_mtime)
        return -1;
    else if(((*one)->fts_statp)->st_mtime < ((*two)->fts_statp)->st_mtime)
        return 1;
    else
        return nameCmp(one,two);
}

/* Comparison based on reverse file modified time (m_time) */
int
revFileModifiedTimeCmp(const FTSENT** one, const FTSENT** two){
    if(((*one)->fts_statp)->st_mtime < ((*two)->fts_statp)->st_mtime)
        return -1;
    else if(((*one)->fts_statp)->st_mtime > ((*two)->fts_statp)->st_mtime)
        return 1;
    else
        return revNameCmp(one,two);
}

/* Comparison based on file access time (a_time) */
int
fileAccessTimeCmp(const FTSENT** one, const FTSENT** two){
    if(((*one)->fts_statp)->st_atime > ((*two)->fts_statp)->st_atime)
        return -1;
    else if(((*one)->fts_statp)->st_atime < ((*two)->fts_statp)->st_atime)
        return 1;
    else
        return nameCmp(one,two);
}

/* Comparison based on reverse file access time (a_time) */
int
revFileAccessTimeCmp(const FTSENT** one, const FTSENT** two){
    if(((*one)->fts_statp)->st_atime > ((*two)->fts_statp)->st_atime)
        return 1;
    else if(((*one)->fts_statp)->st_atime < ((*two)->fts_statp)->st_atime)
        return -1;
    else
        return revNameCmp(one,two);
}

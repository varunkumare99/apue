#ifndef _COMPARE_H_
#define _COMPARE_H_
    
int nameCmp(const FTSENT** one, const FTSENT** two);
int revNameCmp(const FTSENT** one, const FTSENT** two);

int sizeCmp(const FTSENT** one, const FTSENT** two);
int revSizeCmp(const FTSENT** one, const FTSENT** two);

int fileStatusTimeCmp(const FTSENT** one, const FTSENT** two);
int revFileStatusTimeCmp(const FTSENT** one, const FTSENT** two);

int fileModifiedTimeCmp(const FTSENT** one, const FTSENT** two);
int revFileModifiedTimeCmp(const FTSENT** one, const FTSENT** two);

int fileAccessTimeCmp(const FTSENT** one, const FTSENT** two);
int revFileAccessTimeCmp(const FTSENT** one, const FTSENT** two);

#endif

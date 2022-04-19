#ifndef _FLAGS_H_
#define _FLAGS_H_

extern int f_Aflag;         /* List all entries except for '.' and '..' */
extern int f_aflag;         /* Include directory entries whose name begins with a '.' */
extern int f_cflag;         /* use time when file status was last changed */
extern int f_dflag;         /* Directories are listed as plain files(not searched Recursively) */
extern int f_Fflag;         /* display '/' after dir, '*' after exe, '%' after whiteout, '@' after symlink, '=' after socket, '|' after pipi */
extern int f_fflag;         /* output is not sorted */
extern int f_hflag;         /* display sized in human readable format */
extern int f_iflag;         /* for each file, print it's inode */
extern int f_kflag;         /* report sizes in terms of kilo bytes*/
extern int f_lflag;         /* display in long format */
extern int f_nflag;         /* same as -l, except owner and group IDs are displayed numerically */
extern int f_qflag;         /* Force non printable characters in filenames as '?' */
extern int f_Rflag;         /* Recursively list subdirectories encountered */
extern int f_rflag;         /* reverse the order of the sort */
extern int f_Sflag;         /* sort by size, largest file first*/
extern int f_sflag;         /* Display the number of blocks used by each file */
extern int f_tflag;         /* sort by time modified */
extern int f_uflag;         /* sort by time access time */
extern int f_wflag;         /* force raw printing of non printable characters */

extern int w_ownerNameWidth;
extern int w_groupNameWidth;
extern int w_fileSizeWidth;
extern int w_blockSizeWidth;

#endif

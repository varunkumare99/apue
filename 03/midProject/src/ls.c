/*
 * ls - list directory contents
 * ls [-AacdFfhiklnqRrSstuw] [file ...]
 *
 * For each operand that names a file of a type other than directory , ls displays its name as well as any
 * requested, associated information. For each operand that names a file of type directory , ls displays the
 * names of files contained within that directory, as well as any requested, associated information.
 *
 * If no operands are given, the contents of the current directory are displayed. If more than one operand is
 * given, non-directory operands are displayed first; directory and non-directory operands are sorted separately
 * and in lexicographical order.
 */

#include <stdlib.h>
        
int setFlags(int argc, char* argv[]);

int
main(int argc, char *argv[]) {
    setFlags(argc,argv);
    return EXIT_SUCCESS;
}

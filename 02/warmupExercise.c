#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>

void
printSTDconstantValues(){
	printf("STDIN - %d\n",STDIN_FILENO);
	printf("STDOUT - %d\n",STDOUT_FILENO);
	printf("STDERR - %d\n",STDERR_FILENO);
}

void
printSTDfileDescriptorValues(){
       printf("STDIN fd - %d\n", fileno(stdin));
       printf("STDOUT fd - %d\n", fileno(stdout));
       printf("STDERR fd - %d\n", fileno(stderr));
}

void
fopenPrintfd(){
	FILE *fp;

	fp = fopen("temp.txt", "r");
	printf("fd of temp.txt with fopen - %d\n", fileno(fp));
	fclose(fp);
}

void
openPrintfd(){

	int fd;
	
	if ((fd = open("temp.txt", O_WRONLY)) == -1){
		fprintf(stderr, "Unable to open temp.txt: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("fd of temp.txt with open- %d\n", fd);
	if (close(fd) == -1) {
		fprintf(stderr, "Closing failed: %s\n", strerror(errno));
	}

}

void 
openFileMultipleTimes(){
	/* when you open a file, an entry is created in the system wide table of open files.
	 * if a file is opened multiple times before closing then multiple file descriptors are created which point to this entry.
	 */
	
	int fd1,fd2;
	if ((fd1 = open("temp.txt", O_RDONLY)) == -1){
		fprintf(stderr, "Unable to open temp.txt: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}


	if ((fd2 = open("temp.txt", O_RDONLY)) == -1){
		fprintf(stderr, "Unable to open temp.txt: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("fd1 of temp.txt with open multiple times - %d\n", fd1);
	printf("fd2 of temp.txt with open multiple times - %d\n", fd2);
	
	if (close(fd1) == -1) {
		fprintf(stderr, "Closing failed: %s\n", strerror(errno));
	}

	if (close(fd2) == -1) {
		fprintf(stderr, "Closing failed: %s\n", strerror(errno));
	}
}

void
writeToTwoFds(){
	/*Try opening the same file twice, then writing different data to each file descriptor (via write(2)) - what happens? 
	 * the data is overwritten by the file descriptor which writes last.
	 */
	 
	int fd1,fd2;
	char buf[1];
	
	if ((fd1 = open("temp.txt", O_WRONLY)) == -1){
		fprintf(stderr, "Unable to open temp.txt: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}


	if ((fd2 = open("temp.txt", O_WRONLY)) == -1){
		fprintf(stderr, "Unable to open temp.txt: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("fd1 - %d\n", fd1);
	printf("fd2 - %d\n", fd2);

	buf[0] = 'v';
	write(fd1,buf,1);
	buf[0] = 'b';
	write(fd2,buf,2);
	
	if (close(fd1) == -1) {
		fprintf(stderr, "Closing failed: %s\n", strerror(errno));
	}
	if (close(fd2) == -1) {
		fprintf(stderr, "Closing failed: %s\n", strerror(errno));
	}
}

void
alternateRDandWR(){
	int fd,n;
        char buf[1];	

	if ((fd = open("temp1.txt", O_RDWR)) == -1){
		fprintf(stderr, "Unable to open temp.txt: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("fd of temp1.txt with open- %d\n", fd);

	while ((n = read(fd, buf, 1)) > 0) {
		printf("val - %c\n",buf[0]);
		if (write(fd, buf, n) != n) {
			fprintf(stderr, "Unable to write: %s\n",
					strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if (n < 0) {
		fprintf(stderr, "Unable to read: %s\n",
					strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (close(fd) == -1) {
		fprintf(stderr, "Closing failed: %s\n", strerror(errno));
	}
}
int
main(int argc, char** argv) {
	/*cast to void to silence compiler warnings */
	(void) argc;
	(void) argv;

	printSTDconstantValues();
        printSTDfileDescriptorValues();
        openPrintfd();
	fopenPrintfd();
	openFileMultipleTimes();
	writeToTwoFds();
	alternateRDandWR();
	return(EXIT_SUCCESS);
}

#include <err.h>
#include <errno.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MQ_PATH             "/sandwiches"
#define MQ_DEFAULT_PRIORITY 0
#define MQ_IMPORTANT        1

int
main(int argc, char **argv) {
	mqd_t mq;
	int ch, wait;

	wait = 0;
	setprogname(argv[0]);

	while ((ch = getopt(argc, argv, "w")) != -1) {
		switch (ch) {
		case 'w':
			wait = 1;
			break;
		default:
			(void)fprintf(stderr, "Usage: %s [-w] message\n",
					getprogname());
			exit(EXIT_FAILURE);
			/* NOTREACHED */
		}
	}
	argc -= optind;
	argv += optind;

	if ((mq = mq_open(MQ_PATH, O_WRONLY)) == (mqd_t)-1) {
		err(EXIT_FAILURE, "mq_open");
		/* NOTREACHED */
	}

	for (int i = 0; i < argc; i++) {
		if (mq_send(mq, argv[i], strlen(argv[i]), MQ_DEFAULT_PRIORITY) == -1) {
			(void)fprintf(stderr, "%s: Unable to send message %d: %s\n",
					getprogname(), i, strerror(errno));
		}

		if (wait) {
			sleep(1);
		}
	}

	if (mq_send(mq, "TUNA", strlen("TUNA"), MQ_IMPORTANT) == -1) {
		err(EXIT_FAILURE, "mq_send");
		/* NOTREACHED */
	
	}
	if (mq_send(mq, "asTUNA", strlen("asTUNA"), MQ_IMPORTANT) == -1) {
		err(EXIT_FAILURE, "mq_send");
		/* NOTREACHED */
	
	}
	if (mq_close(mq) == -1) {
		err(EXIT_FAILURE, "mq_close");
		/* NOTREACHED */
	}

	return EXIT_SUCCESS;
}

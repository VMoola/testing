#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <string.h>

// (type, nr, datatype)
#define ALLOC _IOWR('a', 1, int)
#define FREE _IOWR('a', 2, int)
#define RD _IOWR('a',3,int)

static int fd;

/*
 * A demo function. This depends on the mmap call mapping a
 * page into userspace so that we can write to it. Every time
 * this function is called, we will mmap, extract the message,
 * then write to it for a new message.
 *
 * We might still have stale data left; this example doesn't
 * do any clearing.
 */
static int mmap_example(char *message)
{
	//mmap example below
	char buf[8];
	int len = 10000;
	char *x = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //mmap
	if (x == MAP_FAILED) {
		printf("mmap dead");
		return -1;
	}

	//read what has been stored for us
	memcpy(buf, x, 8);
	//write our message
	memcpy(x, message, strlen(message));

	munmap(x, len); //no munmap definition, we have to fold it into release
	printf("message from last call:%s\n", buf);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("No device specified\n");
		return -1;
	}

	char *mmap_message;
	char *device = argv[1];
	unsigned int io = 0;
	if (argv[2] != NULL) {
		io = atoi(argv[2]);
		switch (io) {
			case 1:
				io = ALLOC;
				break;
			case 2:
				io = FREE;
				break;
			case 3:
				io = RD;
				break;
		};
	}
	char *args = NULL;
	if (argc > 3)
		args = argv[3];
	if (argc > 4)
		mmap_message = argv[4];

	printf("number of args %d\ndevice: %s\nioctl: %d\nargs: %s\n", argc, device, io, args);

	//assuming our character device was setup at /dev/[name] where name is hello
	fd = open(device, O_RDWR, 0644); //calls open

	if (fd == -1) {
		printf("Failed to open device!\n");
		return -1;
	}

	//ioctl is : (fd, enum, args pointer)
	if (io) {
		unsigned long val = atoi(args); //assuming int for example
		int out = ioctl(fd, io, &val); //calls ioctl

		//Since RW, our ioctl also writes to this variable
		printf("Output value from ioctl %d\n", val);
	}

	mmap_example(mmap_message);
	close(fd); //calls release
	return 0;
}

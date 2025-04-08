#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>

int main()
{
	int len = 10000;
	//assuming our character device was setup at /dev/[name] where name is hello
	int fd = open("/dev/hello", O_RDWR, 0644); //calls open

	if (fd == -1) {
		printf("dead");
		return -1;
	}

	char txt[] = "HO HO HO I'm santa";
	//ioctl is : (fd, enum, args pointer)
	int out = ioctl(fd, 115, txt); //calls ioctl

	if (out < 0)
		printf("something is wrong %d", out);
	else
		printf("theoretical success");

	char *x = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //mmap
	if (!x) {
		printf("mmap dead");
		return -1;
	}

	munmap(x, len); //no munmap definition, we have to fold it into release

	close(fd); //calls release
	return 0;
}

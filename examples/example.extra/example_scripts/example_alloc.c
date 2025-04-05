#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

int main()
{
	int len = 10000;
	int fd = open("testfile", O_RDWR | O_CREAT, 0644);

	if (fd == -1) {
		printf("dead");
		return -1;
	}

	ftruncate(fd, len);
	char txt[] = "example";
	char *x = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (!x) {
		printf("mmap dead");
		return -1;
	}

	close(fd);
	memcpy(x, txt, 5);

	//sleep(100);
	munmap(x, len);

	return 0;
}

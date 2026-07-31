#include <unistd.h>	// For execl
#include <fcntl.h>	// For open
#include <sys/syscall.h>// For syscall

/*
 * Our cpio needs the following 2 files:
 *	- share
 *	---> a.out
 *	---> hello.ko
 * Error handling isn't implemented, so if they are not found, we will
 * simply hang when calling init.
 */

int main(){
	// Loading a module (needs to access the file first)
	int fd = open("/share/hello.ko", O_RDONLY);

	if (fd >= 0) {
		write(1,"Found\n", 6);
		// Raw way to call a syscall - aka no glibc wrapper
		syscall(SYS_finit_module, fd, "", 0);
	}

	// Executing other binaries
	for (int i = 1; i < 5; i++) {
		int pid = fork();
		if (pid == 0)
			execl("/share/a.out", "a.out", (char *)NULL);
	}

	// Hang instead of crash to allow inspection
	pause();
}

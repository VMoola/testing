#include <unistd.h>	// For execl
#include <fcntl.h>	// For open
#include <sys/syscall.h>// For syscall

#include <sys/stat.h>	// For mkdir
#include <sys/mount.h>	// For mount

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

	// populates /dev
	mount("devtmpfs", "/dev", "devtmpfs", 0, NULL);

	mkdir("/mnt", 0755);

	/* This won't work on our base / directory
	 * After this call, we can execute binaries found
	 * within the disk so long as they are statically compiled!
	 *
	 * Dynamically compiled fail due to dependency linking being
	 * filesystem dependent.
	 */
	if(mount("/dev/sda1", "/mnt", "ext4", 0, NULL)) {
		write(1, "Failed mount\n", 13);
	};

	// Executing other binaries
	// Due to our mount above, we can also execute binaries found
	// on the disk if we know the pathway.
	for (int i = 0; i < 1; i++) {
		int pid = fork();
		if (pid == 0)
			execl("/share/a.out", "a.out", (char *)NULL);
	}

	execl("/share/bash", "bash", (char *) NULL);
}

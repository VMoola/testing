#include <unistd.h>	// For execl
#include <fcntl.h>	// For open
#include <sys/syscall.h>// For syscall

#include <sys/stat.h>	// For mkdir
#include <sys/mount.h>	// For mount

/*
 * Our cpio needs the following 2 files:
 *	- share
 *	---> busybox
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
			if (execl("/share/a.out", "a.out", NULL)) {
				// Cleanup children on failure, otherwise...
				// BOOM
				write(1, "Failed to run script\n", 21);
				_exit(0);
			}
	}

	/*
	 * Command arguments get passed in as a NULL terminated string.
	 * We've already mounted our disk. If it has the required setup,
	 * we can chroot into it. If not, we have to access it via a shell.
	 */
	// Uncomment for access to our rdinit environment.
	//execl("/share/busybox", "busybox", "sh", NULL);

	write(1, "Launching Systemd\n", 18);
	int pid = fork();
		if (pid == 0) {
			execl("/share/busybox", "busybox", "touch", "/init", NULL);
		} else {
			/* We must ensure /init is present, so wait on
			 * the child before our switch_root
			 */
			int status;
			syscall(SYS_wait4, pid, &status, 0, NULL);

			execl("/share/busybox", "busybox", "switch_root", "/mnt", "/usr/lib/systemd/systemd", NULL);
		}
}

# Debugging Using Syzbot

For syzbot kernels its as simple as:
1) Grab the bzImage (for booting)
2) Grab the vmlinux (for tracing back addresses)
3) Pass "-serial stdio" into QEMU
4) Wait for the boot
5) Ssh in
6) Grab and run the C reproducer

Syzbot will include the kernel config, so you can build your own kernel
to test your patch locally.

Alternatively, you can mail the patch upstream using #sys-test and inline
the patch to let syzbot do the building and testing for you.

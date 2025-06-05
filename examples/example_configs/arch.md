# Architecture notes

We need a cross-compiler and cross-emulator for different architectures.
We will also need a different initrd/bootable-image for each architecture.

# x86
Here defconfig works.
We get a BzImage located in kernel/arch/x86/boot/BzImage.

# m68k
Here defconfig works.
We get a vmlinux located in kernel/vmlinux.
NUMA is NOT supported on some machines.

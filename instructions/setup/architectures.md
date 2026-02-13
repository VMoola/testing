# Architecture notes

We need a cross-compiler and cross-emulator for different architectures.
We will also need a different initrd/bootable-image for each architecture.

losetup can be used for raw types, mount can be used for iso types.

# x86
Here defconfig works.
We get a BzImage located in kernel/arch/x86/boot/BzImage.

# m68k
Here defconfig works.
We get a vmlinux located in kernel/vmlinux.
NUMA is NOT supported on some machines.
NEEDS to be booted on /dev/sda2
To get a base kernel and initrd to install, grab an iso and mount it, then
copy those and use the iso to install (see the QEMU m68k documentation).
Using a specific machine type makes things easier (debian default is q800).
q800 has a limitation of 1000M!
Display=curses & monitor=vc DOESN'T work (default works though)

## Misc

We can configure dract and use "dracut <output_file>" to generate an
initrd for qemu's parameter if we want to preload modules for the kernel
to have access to. Just pass "initrd=<dracut_file>" to qemu as well.

Dracut can be used to make Unified Kernel Images and is versatile.
Unfortunately, it is very heavyweight and I haven't figured it out yet.

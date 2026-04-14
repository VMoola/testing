# Architecture notes

We need a cross-compiler and cross-emulator for different architectures.
We will also need a different initrd/bootable-image for each architecture.

losetup should be used for images with partition tables, mount can be used
directly for those without.

# x86

Here defconfig works.
We get a BzImage located in kernel/arch/x86/boot/BzImage.

For buildroot:
To boot qemu, here are particular defaults:
1) `append root=/dev/vda` instead of `/dev/sda1`
2) Our input file must have the virtio interface `if=virtio`

# Arm
Arm is 32-bit, aarch64 (arm64) is 64 bit.
Ebbr is for booting (Base Boot Requirements).
Sbsa is for servers (Base System Architecture).
Virt is for vritualization (for software testing).

RISC (Reduced Instruction Set Computer) - less instructions encoded on
the cpu. Basically, the CPU is less proactive to minimize energy costs.

Arm does not do bzImage, just Image.

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

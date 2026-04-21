# Architecture notes

We need a cross-compiler and cross-emulator for different architectures.
We will also need a different initrd/bootable-image for each architecture.

losetup should be used for images with partition tables, mount can be used
directly for those without.

# x86

Here defconfig works.
We get a BzImage located in kernel/arch/x86/boot/BzImage.

For buildroot images:
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

For mkosi images see [mkosi arm64 example](/examples/example_configs/mkosi/booting/qemu_test.conf):
To boot qemu, here are particular defaults:

0) `-a aarch64 #Arch`
1) `-q [anyplaceholder] <- This must follow the arch definition`
2) `-K [path_to_Image] #kernel`
3) `-b [path_to_image.raw] #rootfs`
4) `-R /dev/vda1` instead of `/dev/sda1 #rootfs path`
5) `-i "none,id=hd0" #interface`
6) `-C ttyAMA0 #Console`
7) `-M virt #Machine type`
8) `-cpu cortex-a53 #CPU model`
9) `-device virtio-blk-device,drive=hd0 #tie the device type to the rootfs`

For buildroot images:
0) The same as mkosi except: `-R /dev/vda`

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

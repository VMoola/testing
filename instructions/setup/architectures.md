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

# s390x

s390x focuses on consistent, high throughput workloads.

CISC (Complex Instruction Set Computer) - the CPU does more proactive
work.

Does use a bzImage.

defconfig missing important kernel configs. See [shared filesystem](/instructions/setup/kernel_configs.md)
and [internet functionality](/instructions/setup/kernel_configs.md) sections
in the kernel config instructions.

qemu does NOT support NUMA for s390x.

For mkosi, this command works:
```
qemu-system-s390x -M s390-ccw-virtio -m 4G -smp 4 -kernel ~/kernel/s390x/arch/s390/boot/bzImage -drive file=~/images/mkosi/s390x/image.raw,if=virtio,format=raw -append "rootwait root=/dev/vda1 console=hvc0" -serial stdio -display none -netdev user,id=n1,hostfwd=tcp::10022-:22 -device virtio-net-ccw,netdev=n1 -fsdev local,id=host0,path=mkosi.extra,security_model=passthrough -device virtio-9p-ccw,fsdev=host0,mount_tag=host0
```
TODO: Make a config for our qemu script. This seems to be very particular...
From what I can tell, we can't do the -net nic or -virtfs that we can for
arm64 and x86.

# ppc64-le

ppc64-le focuses on embedded functionality.
pseries is for lighter development (vm).
power9v is for platform development (emulates Bare-Metal).

RISC (Reduced Instruction Set Computer)

Uses vmlinux.

defconfig missing important kernel configs. See [shared filesystem](/instructions/setup/kernel_configs.md)
and [internet functionality](/instructions/setup/kernel_configs.md) sections
in the kernel config instructions.

For mkosi, this command works:
```
qemu-system-ppc64 -M pseries,x-vof=on -cpu POWER8 -m 1G -smp 4 -kernel ~/kernel/ppc64/vmlinux -append "console=hvc0 rootwait root=/dev/sda1" -drive file=~/images/mkosi/ppc64/image.raw,if=scsi,index=0,format=raw -serial stdio -display none -netdev user,id=n1,hostfwd=tcp::10022-:22 -device virtio-net-pci,netdev=n1 -virtfs local,path=mkosi.extra,mount_tag=host0,security_model=passthrough,id=host0
```
TODO: Make a config for our qemu script. This seems to be very particular...
From what I can tell, we can't do the -net nic that we can for arm64 and x86.
Doing it the similar to s390x appears to work though.

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
